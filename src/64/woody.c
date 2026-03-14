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

	int fd = open(filename, O_RDWR);
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
	e->file_map = get_file_in_a_map_write_64(fd, e->file_size);
	if (!e->file_map)
	{
		close (fd);
		return (1);
	}
	close (fd);
	e->elf_header = get_elf_header_64(e);
	if (!e->elf_header)	
		return (1);

	e->sectionsHeader = get_sections_header_64(e);
	if (!e->sectionsHeader)
		return (1);

	e->shstrtab = get_section_by_header_64(e, &e->sectionsHeader[e->elf_header->e_shstrndx]);
	if (!e->shstrtab)
		return (1);
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

int look_for_zero_padding(t_elf64 *e)
{
	int k;
	for (int i = 0; i <= e->elf_header->e_shnum; i++)
	{
		if (e->sectionsHeader[i].sh_flags & SHF_EXECINSTR)
		{
			printf("%s\n", e->sectionsHeader[i].sh_name + e->shstrtab);

			printf("offset; %lu  size; %lu  entsize; %lu\n", e->sectionsHeader[i].sh_offset, e->sectionsHeader[i].sh_size, e->sectionsHeader[i].sh_entsize);
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

	look_for_zero_padding(&e);
	
	return (0);
}
