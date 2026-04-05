#ifndef WOODY64_HPP
# define WOODY64_HPP

#include "elf.h"
#include "libft.h"

#include <stdio.h>

typedef struct s_payload
{
	unsigned char	*payload;
	int	payload_size;
	int key_size;
} t_payload;

typedef struct s_injection
{
	Elf64_Phdr	*segment;
	uint64_t	old_entry_point;
	uint64_t injection_offset;
	uint64_t injection_vaddr;
	uint64_t text_vaddr;
	int text_size;

} t_injection;

typedef struct s_elf
{
	char *filename;
	long	file_size;
	void	*file_map;
	Elf64_Ehdr	*elf_header;
	Elf64_Shdr	*sectionsHeader;
	char	*shstrtab;
	int fd;
} t_elf;

typedef struct s_woody
{
	t_elf elf;
	t_payload payload;
	t_injection injection;

} t_woody;

int	find_class(char *filename);
int	woody_64(char *filename);

void	*get_file_in_a_map_64(int fd, int file_size);
void	*get_file_in_a_map_write_64(int fd, int file_size);
Elf64_Ehdr	*get_elf_header_64(t_woody *e);
Elf64_Shdr	*get_sections_header_64(t_woody *e);
Elf64_Shdr	*get_section_header_by_name_64(t_woody *e, const char *name);

char	*get_section_by_header_64(t_woody *e, Elf64_Shdr *sectionHeader);
// char	*get_section_by_name_64(t_woody *e, const char *name);

int injection(t_woody *e);
int	segment_is_PT_LOAD_and_PF_X(Elf64_Phdr *segment);
int get_injection_offset(t_woody *e);
int	segment_is_PT_NOTE(Elf64_Phdr *segment);
int	enough_zero_padding_in_segment(Elf64_Phdr *segment, int size); // to check;
int read_payload(t_woody *e);
int check_file(char *filename);
int	init_elf_64(t_woody *e, char *filename);
int injection (t_woody *e);
int copy_into_woody(t_woody *e);
int patch_payload(t_woody *e);
int	xor_point_text(t_woody *w);

//Antoine
uint64_t  insert_payload(t_woody *e);

#endif
