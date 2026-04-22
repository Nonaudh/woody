#include "woody.h"
#include "fcntl.h"
#include <sys/mman.h>
#include <stdio.h>

int flag_key(char *argv)
{
	int	i;
	int wrong_flag = 0;
	if (argv && ft_strchr(argv, '-') == argv && ft_strlen(argv) != 1)
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
		flag = flag_key(argv[i]);
		if (flag == -1)
			break ;
		if (flag == 1)
		{
			if (argv[i + 1] && ft_strchr(argv[i + 1], '-') != argv[i + 1])
			{
				i++;
				if (ft_strlen(argv[i]))
				{
					w->payload.key_as_param = 1;
					w->payload.payload = (unsigned char *)argv[i];
				}
			}
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

void	bzero_struct(t_woody *w)
{
	w->elf.file_map = NULL;
	w->payload.payload = NULL;
	w->payload.key_as_param = 0;
}

void	print_key(t_woody *w)
{
	printf("KEY: ");
	for (int i = w->payload.payload_size; i < w->payload.payload_size + w->payload.key_size; i++)
		printf("%x ", w->payload.payload[i]);
	printf("\n");
}
