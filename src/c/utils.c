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

int flag_key(int argc, char *argv)
{
	int	i;
	int wrong_flag = 0;
	if (argv && ft_strchr(argv, '-') == argv)
	{
		for (i = 1; i < ft_strlen(argv); i++)
		{
			if (argv[i] != 'k')
			{
				wrong_flag = 1;
				break ;
			}
		}
	}
	else
		return (0);
	if (wrong_flag)
	{
		ft_dprintf(2, "woody: wrong option: %c\n", argv[i]);
		return (-1);
	}
	return (1);
}

int check_args(t_woody *w, int argc, char **argv)
{
	int	i;
	int flag;

	if (argc < 2)
		return (1);
	for (i = 1; i < argc; i++)
	{
		flag = flag_key(argc, argv[i]);
		if (flag == -1)
			break ;
		if (flag == 1)
		{
			if (argv[i + 1] && ft_strchr(argv[i + 1], '-') != argv[i + 1])
				w->payload.payload = (unsigned char *)argv[++i];
		}
		else
			w->elf.filename = argv[i];
	}
	if (flag == -1)
		return (1);
	if (!w->elf.filename)
	{
		ft_dprintf(2, "woody: no file given\n");
		return (1);
	}
	return (0);
}

void	bzero_ptr(t_woody *w)
{
	w->payload.payload = NULL;
}


void	print_key(t_woody *w)
{
	printf("KEY: ");
	for (int i = w->payload.payload_size; i < w->payload.payload_size + w->payload.key_size; i++)
		printf("%x ", w->payload.payload[i]);
	printf("\n");
}
