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
	uint64_t	entry_point;
	int	payload_size;
	char	*shstrtab;
	uint64_t text_vaddr;
	int text_size;
	unsigned char *key;
	int key_size;
	//C'est antoine
	// int elf_entre;
	int fd;

} t_elf64;

int	find_class(char *filename);
int	woody_64(char *filename);

void	*get_file_in_a_map_64(int fd, int file_size);
void	*get_file_in_a_map_write_64(int fd, int file_size);
Elf64_Ehdr	*get_elf_header_64(t_elf64 *e);
Elf64_Shdr	*get_sections_header_64(t_elf64 *e);
Elf64_Shdr	*get_section_header_by_name_64(t_elf64 *e, const char *name);

char	*get_section_by_header_64(t_elf64 *e, Elf64_Shdr *sectionHeader);
// char	*get_section_by_name_64(t_elf64 *e, const char *name);

int injection(t_elf64 *e);
int	segment_is_PT_LOAD_and_PF_X(Elf64_Phdr *segment);
uint64_t get_injection_offset(t_elf64 *e, unsigned char *code, int size);
int	segment_is_PT_NOTE(Elf64_Phdr *segment);
int	enough_zero_padding_in_segment(Elf64_Phdr *segment, int size); // to check;

//Antoine
uint64_t  insert_payload(t_elf64 *e);

#endif
