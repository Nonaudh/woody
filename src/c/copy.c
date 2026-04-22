#include "woody.h"
#include <fcntl.h>
#include <sys/mman.h>

int copy_into_woody(t_woody *w)
{
	void *tmp = malloc(sizeof(char) * w->elf.file_size);
	if (!tmp)
		return (1);
	ft_memcpy(tmp, w->elf.file_map, w->elf.file_size);
	int woody_fd = open("woody", O_CREAT | O_RDWR | O_TRUNC, 0755);
	if (woody_fd == -1)
		return (1);
	write(woody_fd, tmp, w->elf.file_size);
	close(woody_fd);
	free(tmp);
	return (0);
}
