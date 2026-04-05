#include "woody64.h"
#include "fcntl.h"
#include <sys/mman.h>
#include <stdio.h>

long get_file_size(t_woody *w)
{
	long file_size = lseek(w->elf.fd, 0, SEEK_END);
	lseek(w->elf.fd, 0, SEEK_CUR);
	return (file_size);
}

int	init_elf_64(t_woody *w, char *filename)
{
	w->elf.file_map = NULL;
	w->elf.fd = open(filename, O_RDWR);
	if (w->elf.fd == -1)
	{
		perror(filename);
		return (1);
	}
	w->elf.filename = filename;
	w->elf.file_size = get_file_size(w);
	w->elf.file_map = get_file_in_a_map_64(w->elf.fd, w->elf.file_size);
	if (!w->elf.file_map)
	{
		close (w->elf.fd);
		return (1);
	}
	w->elf.elf_header = get_elf_header_64(w);
	if (!w->elf.elf_header)	
		return (1);
	w->elf.sectionsHeader = get_sections_header_64(w);
	if (!w->elf.sectionsHeader)
		return (1);
	w->elf.shstrtab = get_section_by_header_64(w, &w->elf.sectionsHeader[w->elf.elf_header->e_shstrndx]);
	if (!w->elf.shstrtab)
		return (1);
	// w->entry_point = w->elf_header->e_entry;
	// munmap(map, w->file_size);
	return (0);
}

int check_file(char *filename)
{
	// to code
	return (0);
}
