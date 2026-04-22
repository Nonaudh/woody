#include "woody.h"
#include <sys/mman.h>

void	*get_file_in_a_map_64(int fd, int file_size)
{
	void	*map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
		return (NULL);
	return (map);
}

Elf64_Ehdr	*get_elf_header_64(t_woody *w)
{
	Elf64_Ehdr	*elf_header;

	elf_header = (Elf64_Ehdr *)w->elf.file_map;

	return (elf_header);
}

Elf64_Shdr	*get_sections_header_64(t_woody *w)
{
	if (w->elf.elf_header->e_shoff + (w->elf.elf_header->e_shnum * sizeof(Elf64_Shdr)) > (unsigned long)w->elf.file_size
		|| w->elf.elf_header->e_phoff + (w->elf.elf_header->e_phnum * sizeof(Elf64_Phdr)) > (unsigned long)w->elf.file_size)
	{
		ft_dprintf(2, "woody: %s: file format not recognized\n", w->elf.filename);
		return (NULL);
	}
	return (w->elf.file_map + w->elf.elf_header->e_shoff);
}

char	*get_section_by_name_64(t_woody *w, const char *name)
{
	int i = 1;
	char *ptr;

	while (i < w->elf.elf_header->e_shnum)
	{
		if (!ft_strncmp(name, w->elf.shstrtab + w->elf.sectionsHeader[i].sh_name + 1, ft_strlen(name)) 
			&& ft_strlen(name) == ft_strlen(w->elf.shstrtab + w->elf.sectionsHeader[i].sh_name + 1))
			break ;
		i++;
	}
	if (i == w->elf.elf_header->e_shnum)
	{
		return (NULL);
	}
	ptr = get_section_by_header_64(w, &w->elf.sectionsHeader[i]);
	return (ptr);
}

Elf64_Shdr	*get_section_header_by_name_64(t_woody *w, const char *name)
{
	int i = 1;

	while (i < w->elf.elf_header->e_shnum)
	{
		if (!ft_strncmp(name, w->elf.shstrtab + w->elf.sectionsHeader[i].sh_name + 1, ft_strlen(name)) 
			&& ft_strlen(name) == ft_strlen(w->elf.shstrtab + w->elf.sectionsHeader[i].sh_name + 1))
			break ;
		i++;
	}
	if (i == w->elf.elf_header->e_shnum)
		return (NULL);
	return (&w->elf.sectionsHeader[i]);
}

char	*get_section_by_header_64(t_woody *w, Elf64_Shdr *sectionHeader)
{
	if (!sectionHeader || sectionHeader->sh_offset + sizeof(Elf64_Shdr) > (unsigned long)(w->elf.file_size + w->elf.file_map))
		return (NULL);

	char *ptr = w->elf.file_map + sectionHeader->sh_offset;

	return (ptr);
}
