#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>

long	size_of_file_64(int fd, char *filename)
{
	struct stat stat;

	if (fstat(fd, &stat))
		return (-1);
	if (stat.st_size < (long)sizeof(Elf64_Ehdr))
	{
		ft_dprintf(2, "nm: %s: file format not recognized\n", filename);
		return (-1);
	}
	return (stat.st_size);
}

int	init_elf_64(t_elf64 *e, char *filename)
{
	e->file_map = NULL;
	e->fd = open(filename, O_RDWR);
	if (e->fd == -1)
	{
		perror(filename);
		return (1);
	}
	e->filename = filename;
	e->file_map = get_file_in_a_map_write_64(e->fd, e->file_size);
	if (!e->file_map)
	{
		close (e->fd);
		return (1);
	}
	e->elf_header = get_elf_header_64(e);
	if (!e->elf_header)	
		return (1);
	e->sectionsHeader = get_sections_header_64(e);
	if (!e->sectionsHeader)
		return (1);
	// munmap(map, e->file_size);
	
	e->shstrtab = get_section_by_header_64(e, &e->sectionsHeader[e->elf_header->e_shstrndx]);
	if (!e->shstrtab)
		return (1);
	return (0);
}
int copy_elf_64(t_elf64 *e, char *filename)
{
    e->file_map = NULL;

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror(filename);
        return (1);
    }
    e->filename = filename;

    e->file_size_pre_hand = lseek(fd, 0, SEEK_END);
e->file_size = e->file_size_pre_hand 
             + e->size_code 
             + sizeof(uint64_t)          // magic marker
             + sizeof(uint32_t)          // table_count
             + (sizeof(t_table_haufman) * 76);

    if (e->file_size == 0)
    {
        close(fd);
        return (1);
    }

    e->file_map = get_file_in_a_map_64(fd, e->file_size_pre_hand);
    if (!e->file_map)
    {
        close(fd);
        return (1);
    }
    close(fd);

    int woody_fd = open("woody", O_CREAT | O_RDWR | O_TRUNC, 0755);
    if (woody_fd == -1)
        return (1);

    // Ecrit le fichier original
    write(woody_fd, e->file_map, e->file_size_pre_hand);

    // Etend avec des zeros
char zero[e->size_code + sizeof(uint64_t) + sizeof(uint32_t) + (sizeof(t_table_haufman) * 76)];
ft_memset(zero, 0, sizeof(zero));
write(woody_fd, zero, sizeof(zero));

    munmap(e->file_map, e->file_size_pre_hand);
    close(woody_fd);
    return (0);
}
int insert_pt_load(t_elf64 *e,Elf64_Phdr *Phdr, char *code, int lencode, int i)
{
	uint64_t fin_segment = Phdr[i].p_offset + Phdr[i].p_filesz;
	uint64_t padding = Phdr[i + 1].p_offset - fin_segment;
	uint64_t new_entry_point = Phdr[i].p_vaddr + Phdr[i].p_filesz;
	if (padding < (int64_t)lencode)
		return(1);
	Phdr[i].p_filesz += lencode;	
	Phdr[i].p_memsz  += lencode;
	
	ft_memcpy((char *)e->file_map  + fin_segment, code, lencode);
	ft_memcpy((char *)e->file_map  + 0x18, &new_entry_point, 8);
	return(0);
}
int insert_pt_note(t_elf64 *e, Elf64_Phdr *Phdr, int i)
{
    int lencode = e->size_code
                + sizeof(uint64_t)           // magic
                + sizeof(uint32_t)           // table_count
                + (sizeof(t_table_haufman) * 76);

    uint64_t offset    = e->file_size_pre_hand;
    uint64_t new_vaddr = 0x5000 + (offset & 0xFFF);

    Phdr[i].p_type   = PT_LOAD;
    Phdr[i].p_flags  = PF_R | PF_X;
    Phdr[i].p_offset = offset;
    Phdr[i].p_vaddr  = new_vaddr;
    Phdr[i].p_paddr  = new_vaddr;
    Phdr[i].p_filesz = lencode;
    Phdr[i].p_memsz  = lencode;
    Phdr[i].p_align  = 0x1000;

    ft_memcpy((char *)e->file_map + 0x18, &new_vaddr, 8);
    return (0);
}

int insert_payload(t_elf64 *e)
{
	char code[] ="\x31\xc0\x99\xb2\x0a\xff\xc0\x89\xc7\x48\x8d\x35\x12\x00\x00\x00\x0f\x05"
    "\xb2\x2a\x31\xc0\xff\xc0\xf6\xe2\x89\xc7\x31\xc0\xb0\x3c\x0f\x05\x2e\x2e"
    "\x57\x4f\x4f\x44\x59\x2e\x2e\x0a";
	int lencode = sizeof(code)  - 1;
	int i;
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	int save ;
	i = 0;
	while(i < e->elf_header->e_phnum - 1)//Elfique transformation n'est pas finit
	{
		printf("i=|%d| phadr aligndb|%lu|", i, Phdr[i].p_align);
		// if(Phdr[i].p_type == PT_LOAD && Phdr[i].p_flags &  PF_X)
		// {
		// 	if(insert_pt_load(e, Phdr, code, lencode, i) == 0)
		// 		break;
		// }
		if (Phdr[i].p_type == PT_NOTE && Phdr[i].p_align == 0x4)
		{
			if(insert_pt_note(e, Phdr, i) == 0)
				break;
		}
		i++;
	}
	return (0);
}
void print_byte_bits(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
        printf("%d", (byte >> i) & 1);
}

int print_fd_bits(t_elf64 *e)
{
	
	unsigned char buf[4096];
    ssize_t       n;
    size_t        byte_count = 0;
	
    // Rewind au début du fichier
    int t = lseek(e->fd, 0, SEEK_SET);
	// printf("\nZZZ|fd|%d|lseekk|%d|\n", e->fd, t) ;

    while ((n = read(e->fd, buf, sizeof(buf))) > 0)
    {
        for (ssize_t i = 0; i < n; i++)
        {
            // Offset tous les 4 octets pour lisibilité
            if (byte_count % 4 == 0)
                printf("\n[0x%06zx] ", byte_count);
            else
                printf(" ");

            print_byte_bits(buf[i]);
            byte_count++;
        }
    }
	return(0);
    printf("\n");
}


unsigned char	*get_section_by_header_64_unsigned_char(t_elf64 *e, Elf64_Shdr *sectionHeader)
{
	if (sectionHeader->sh_offset + sectionHeader->sh_size > (unsigned long)e->file_size)
			return (NULL);

	unsigned char *ptr = e->file_map + sectionHeader->sh_offset;
	return (ptr);
}

char	*get_section_by_header_64(t_elf64 *e, Elf64_Shdr *sectionHeader)
{
if (sectionHeader->sh_offset + sectionHeader->sh_size > (unsigned long)e->file_size)
		return (NULL);

	char *ptr = e->file_map + sectionHeader->sh_offset;

	return (ptr);
}
Elf64_Shdr	*get_section_header_by_name_64(t_elf64 *e, const char *name)
{
	int i = 1;

	while (i < e->elf_header->e_shnum)
	{
		if (!ft_strncmp(name, e->shstrtab + e->sectionsHeader[i].sh_name, ft_strlen(name)) 
			&& ft_strlen(name) == ft_strlen(e->shstrtab + e->sectionsHeader[i].sh_name))
			break ;
		i++;
	}
	if (i == e->elf_header->e_shnum)
	{
		return (NULL);
	}
	return (&e->sectionsHeader[i]);
}

int woody_64(char *filename)
{
    t_elf64 e;
unsigned char shellcode[] = {
    0xE8, 0x00, 0x00, 0x00, 0x00,   // call next
    0x5B,                            // pop rbx

    0x44, 0x8B, 0x6B, 0x00,         // mov r13d, [rbx + offset_count]
    0x4C, 0x8D, 0x73, 0x00,         // lea r14,  [rbx + offset_table]

    0xB8, 0x01, 0x00, 0x00, 0x00,   // mov eax, 1
    0xBF, 0x01, 0x00, 0x00, 0x00,   // mov edi, 1
    0x4C, 0x89, 0xF6,               // mov rsi, r14
    0xBA, 0x08, 0x00, 0x00, 0x00,   // mov edx, 8
    0x0F, 0x05,                      // syscall
    0x49, 0x83, 0xC6, 0x08,         // add r14, 8
    0x41, 0xFF, 0xCD,               // dec r13d
    0x75, 0xE3,                      // jne loop
    0xCC,                            // int3
};

    // rbx pointe sur "pop rbx" = byte 5
    // offset depuis rbx jusqu'a magic = sizeof(shellcode) - 5
    // offset depuis rbx jusqu'a count = sizeof(shellcode) - 5 + sizeof(uint64_t)
    // offset depuis rbx jusqu'a table = sizeof(shellcode) - 5 + sizeof(uint64_t) + sizeof(uint32_t)
    int offset_count = sizeof(shellcode) - 5 + sizeof(uint64_t);
    int offset_table = offset_count + sizeof(uint32_t);

    printf("sizeof shellcode = %zu\n", sizeof(shellcode));
    printf("offset_count     = %d\n",  offset_count);
    printf("offset_table     = %d\n",  offset_table);

    // Patch les offsets dans le shellcode
    shellcode[9]  = (unsigned char)offset_count;
    shellcode[13] = (unsigned char)offset_table;

    e.size_code = sizeof(shellcode);
    e.shellcode = shellcode;

    if (copy_elf_64(&e, filename))
        return (1);
    if (init_elf_64(&e, "woody"))
        return (1);

    insert_payload(&e);
    compression_decompression(&e);
    close(e.fd);
    munmap(e.file_map, e.file_size);
    return (0);
}