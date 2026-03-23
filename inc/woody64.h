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
	//C'est antoine
	int elf_entre;
	int fd;
} t_elf64;

int	find_class(char *filename);
int	woody_64(char *filename);

void	*get_file_in_a_map_64(int fd, int file_size);
void	*get_file_in_a_map_write_64(int fd, int file_size);
Elf64_Ehdr	*get_elf_header_64(t_elf64 *e);
Elf64_Shdr	*get_sections_header_64(t_elf64 *e);

char	*get_section_by_header_64(t_elf64 *e, Elf64_Shdr *sectionHeader);
char	*get_section_by_name_64(t_elf64 *e, const char *name);
Elf64_Shdr	*get_section_header_by_name_64(t_elf64 *e, const char *name);

//Antoine
int insert_payload(t_elf64 *e);
int insert_p_load(t_elf64 *e,Elf64_Phdr *Phdr, char *code, int lencode, int i);
int add_program_header(t_elf64 *e, int lencode, Elf64_Phdr *phdr);
int change_all(t_elf64 *e, int lencode, Elf64_Phdr *phdr, int i);




#endif
