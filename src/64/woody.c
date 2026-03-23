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

	e->file_size = size_of_file_64(e->fd, filename);
	if (e->file_size == -1)
	{
		close (e->fd);
		return (1);
	}
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
	
		
		
	// printf("Je suis le nombre de Programme header %d", e->elf_header->e_phnum);//Mise en place d'une conditions ca le nombre peux se trouver a un autre endroits
	e->shstrtab = get_section_by_header_64(e, &e->sectionsHeader[e->elf_header->e_shstrndx]);
	if (!e->shstrtab)
		return (1);
	// munmap(map, e->file_size);
	return (0);
}

// int	print_section_text(t_elf64 *e)
// {
// 	Elf64_Shdr	*textHeader = get_section_header_by_name_64(e, ".text");
// 	unsigned char	*text = (unsigned char *)get_section_by_header_64(e, textHeader);

// 	for (int i = 0; i < textHeader->sh_size; i++)
// 	{
// 		// printf("%02x ", text[i]);
// 		// if ((i + 1) % 16 == 0)
// 		// 	printf("i\n");
// 		text[i] = 42;
// 	}
// 	printf("\n");

// 	return (0);
// }
int	copy_elf_64(t_elf64 *e, char *filename)
{
	e->file_map = NULL;

	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror(filename);
		return (1);
	}
	e->filename = filename;

	e->file_size = size_of_file_64(fd, filename);
	if (e->file_size == -1)
	{
		close (fd);
		return (1);
	}
	e->file_map = get_file_in_a_map_64(fd, e->file_size);
	if (!e->file_map)
	{
		close (fd);
		return (1);
	}
	close (fd);
	int woody_fd = open("woody", O_CREAT | O_RDWR | O_TRUNC, 0755);
	if (woody_fd == -1)
		return (1);
	write(woody_fd, e->file_map, e->file_size);
	munmap(e->file_map, e->file_size);
	close(woody_fd);
	return (0);
}
int insert_pt_load(t_elf64 *e,Elf64_Phdr *Phdr, char *code, int lencode, int i)
{
	uint64_t fin_segment = Phdr[i].p_offset + Phdr[i].p_filesz;
	uint64_t padding = Phdr[i + 1].p_offset - fin_segment;
	uint64_t new_entry_point = Phdr[i].p_vaddr + Phdr[i].p_filesz;
	if (padding < (int64_t)lencode)//Il faur verifier cette conditions 
		return(1);
	Phdr[i].p_filesz += lencode;	
	Phdr[i].p_memsz  += lencode;
	void    *mapped;
	
	//Pour arnaud 
	off_t   offset_aligne = (fin_segment / Phdr[i].p_align) * Phdr[i].p_align;
	off_t delta = fin_segment - offset_aligne;
	mapped = mmap(0, lencode, PROT_READ | PROT_WRITE, MAP_SHARED, e->fd, offset_aligne);
	if (mapped == MAP_FAILED)
    	return (3);
	ft_memcpy(mapped + delta, code, lencode);
	munmap(mapped, lencode);
	void *mapped_entry_point;
	off_t ofsset_align_entry_point = (0x18/ Phdr[i].p_align) * Phdr[i].p_align;
	off_t delta_entry_point = 0x18 - ofsset_align_entry_point ;
	mapped_entry_point = mmap(0, 8, PROT_READ | PROT_WRITE, MAP_SHARED, e->fd, ofsset_align_entry_point);
	if (mapped == MAP_FAILED)
    	return (3);
		
	ft_memcpy(mapped_entry_point + delta_entry_point, &new_entry_point, 8);
	munmap(mapped_entry_point, 8);
	return(0);
	
	//De antoine 
	// lseek(e->fd, fin_segment, SEEK_SET);
	// write(e->fd, code, lencode);
	// lseek(e->fd, 0x18, SEEK_SET);
	// write(e->fd, &new_entry_point, 8);
	return(0);	
}

int insert_pt_note(t_elf64 *e,Elf64_Phdr *Phdr, char *code, int lencode, int i, int page_size)//Il fauut mettre une conditions sur la len
{
	//PAGE PRIT SUR ANCIEN PT_LOAD
	Phdr[i].p_type = PT_LOAD;
	Phdr[i].p_flags = PF_R | PF_X; 

	Phdr[i].p_filesz += lencode;	
	Phdr[i].p_memsz  += lencode;
	
	//Pour arnaud 
	void *mapped;
	off_t offset = (Phdr[i].p_offset  / page_size) * page_size;
	off_t  delta  = Phdr[i].p_offset  - offset;
	mapped = mmap (0,lencode,  PROT_READ | PROT_WRITE, MAP_SHARED, e->fd, offset);
	ft_memcpy((char *)mapped + delta , code, lencode);
	void *mapped_entry_point;
	off_t ofsset_align_entry_point = (0x18/ page_size) * page_size;
	off_t delta_entry_point = 0x18 - ofsset_align_entry_point ;
	mapped_entry_point = mmap(0, 8, PROT_READ | PROT_WRITE, MAP_SHARED, e->fd, ofsset_align_entry_point);
	if (mapped == MAP_FAILED)
    	return (3);
	ft_memcpy(mapped_entry_point + delta_entry_point, &Phdr[i].p_vaddr, 8);
	munmap(mapped_entry_point, 8);
	return(0);
	
	//De antoine 
	// lseek(e->fd, Phdr[i].p_offset, SEEK_SET);
	// write(e->fd, code, lencode);			
	// lseek(e->fd, 0x18, SEEK_SET);
	// write(e->fd, &Phdr[i].p_vaddr , 8);
	return(0);
	
}

int printf_All(t_elf64 *e, int lencode, Elf64_Phdr *phdr)
{
	//1 er chose il faut boucler  sur le programme header
	printf("\nVoila size progheader|%d| &&Start sectionHeader|%lu| Start ProgrammeHeader|%lu| && l\n ", e->elf_header->e_phentsize, e->elf_header->e_shoff,  e->elf_header->e_phoff);
	int i = 0;
	printf("\nBoucle phdr\n");
	while(i < e->elf_header->e_phnum)
	{
		printf("i = |%d| Type |%d| Voila size , filepagephdr|%lu|, start Phdr|%lu|, taillePlace|%lu|\n",i, phdr[i].p_type,  phdr[i].p_filesz, phdr[i].p_offset, phdr[i].p_memsz);
		i++;
	}
	i = 0;
	printf("\nBoucle section header\n");
	while(i < e->elf_header->e_shnum)
	{
		printf("i = |%d|, EntrytABLEAUsi sybole==|%lu|,EntrY section|%lu|Taille de la section |%lu|Adresse de la section |%lu|\n",i, e->sectionsHeader[i].sh_entsize, e->sectionsHeader[i].sh_offset, e->sectionsHeader[i].sh_size, e->sectionsHeader[i].sh_addr);
		i++;
	}
	return(0);
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
	while(i < e->elf_header->e_phnum - 1)
	{
		if (Phdr[i].p_type == PT_LOAD)
			save = Phdr[i].p_align;
		printf("i=|%d| phadr align|%lu|", i, Phdr[i].p_align);
		if(Phdr[i].p_type == PT_LOAD && Phdr[i].p_flags &  PF_X)
		{
			if(insert_pt_load(e, Phdr, code, lencode, i) == 0)
			return(0);
		}
		if(Phdr[i].p_type == PT_NOTE)
		{
			if(insert_pt_note(e, Phdr, code, lencode, i, save)==0)
				return(0);
		}
		i++;
	}
	close(e->fd);//si break attention 
	return (0);
}

int	woody_64(char *filename)
{
	t_elf64 e;
	
	if (copy_elf_64(&e, filename))
		return (1);

	if (init_elf_64(&e, "woody"))
		return (1);

	insert_payload(&e);
	munmap(e.file_map, e.file_size);
	return (0);
}
