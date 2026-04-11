#include "woody64.h"
#include <fcntl.h>

int	not_an_elf(char *map, char *filename)
{
	if (map[0] != 0x7f || map[1] != 'E' || map[2] != 'L' || map[3] != 'F')
	{
		ft_dprintf(2, "nm: %s: file format not recognized\n", filename);
		return (1);
	}
	return (0);
}

long get_file_size(t_woody *w)
{
	long file_size = lseek(w->elf.fd, 0, SEEK_END);
	lseek(w->elf.fd, 0, SEEK_CUR);
	return (file_size);
}

int	init_elf_64(t_woody *w)
{
	w->elf.file_map = NULL;
	w->elf.fd = open(w->elf.filename, O_RDWR);
	if (w->elf.fd == -1)
	{
		perror(w->elf.filename);
		return (1);
	}
	w->elf.file_size = get_file_size(w);
	w->elf.file_map = get_file_in_a_map_64(w->elf.fd, w->elf.file_size);
	close (w->elf.fd);
	if (!w->elf.file_map)
		return (1);
	w->elf.elf_header = get_elf_header_64(w);
	if (!w->elf.elf_header)
		return (1);
	w->elf.sectionsHeader = get_sections_header_64(w);
	if (!w->elf.sectionsHeader)
		return (1);
	w->elf.shstrtab = get_section_by_header_64(w, &w->elf.sectionsHeader[w->elf.elf_header->e_shstrndx]);
	if (!w->elf.shstrtab)
		return (1);
	return (0);
}
