#ifndef WOODY64_HPP
# define WOODY64_HPP

#include "elf.h"
#include "libft.h"

typedef struct s_elf64
{
	char *filename;
	long	file_size;
	void	*file_map;
	Elf64_Ehdr	*elf_header;
	Elf64_Shdr	*sectionsHeader;
	char	*shstrtab;
} t_elf64;

int	find_class(char *filename);
int	woody64(char *filename);

void	*get_file_in_a_map_64(int fd, int page_size);
Elf64_Ehdr	*get_elf_header_64(t_elf64 *e);
Elf64_Shdr	*get_sections_header_64(t_elf64 *e);

char	*get_section_by_header_64(t_elf64 *e, Elf64_Shdr *sectionHeader);
char	*get_section_by_name_64(t_elf64 *e, const char *name);
Elf64_Shdr	*get_section_header_by_name_64(t_elf64 *e, const char *name);

#endif