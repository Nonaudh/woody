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
	// e->shstrtab = get_section_by_header_64(e, &e->sectionsHeader[e->elf_header->e_shstrndx]);
	// if (!e->shstrtab)
	// 	return (1);
	// e->entry_point = e->elf_header->e_entry;
	// munmap(map, e->file_size);
	return (0);
}
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

	e->file_size =  lseek(fd, 0, SEEK_END);
	if (e->file_size == 0)//Il faut verifier cette conditions 
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
	// munmap(e->file_map, e->file_size);
	close(woody_fd);
	return (0);
}

void xor_xor(char *text, int size, unsigned char *key, int key_size)
{
	int j = 0;

	for (int i = 0; i < size; i++)
	{
		if (j == key_size)
			j = 0;
		text[i] ^= key[j];
		j++;
	}
}

int	xoring_PT_LOAD(t_elf64 *e)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	int i;
	int found = 0;
	e->key = (unsigned char *)"AKEY";
	e->key_size = ft_strlen((char *)e->key);

	for (i = 0; !found && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD_and_PF_X(&Phdr[i]))
		{
			e->PT_LOAD_vaddr = Phdr[i].p_vaddr;
			e->PT_LOAD_size = Phdr[i].p_filesz;
			xor_xor(e->file_map + Phdr[i].p_offset, Phdr[i].p_filesz, e->key, e->key_size);
			found = 1;
		}
	}
	return (0);
}

int	woody_64(char *filename)
{
	t_elf64 e;
	
	if (copy_elf_64(&e, filename))
		return (1);

	if (init_elf_64(&e, "woody"))
		return (1);

	if (xoring_PT_LOAD(&e))
		return (1);

	if (injection(&e))
		return (1);
	
	munmap(e.file_map, e.file_size);
	return (0);
}
