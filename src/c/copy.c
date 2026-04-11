#include "woody64.h"
#include <fcntl.h>
#include <sys/mman.h>

int copy_into_woody(t_woody *w)
{
	int woody_fd = open("woody", O_CREAT | O_RDWR | O_TRUNC, 0755);
	if (woody_fd == -1)
		return (1);
	write(woody_fd, w->elf.file_map, w->elf.file_size);
	close(woody_fd);
	return (0);
}
