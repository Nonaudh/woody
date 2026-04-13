#include "woody64.h"
#include <fcntl.h>
#include <sys/mman.h>

int copy_into_woody(t_woody *w, t_table_haufman *table)
{
	int woody_fd = open("woody", O_CREAT | O_RDWR | O_TRUNC, 0755);
	if (woody_fd == -1)
		return (1);
	write(woody_fd, w->elf.file_map, w->elf.file_size);
	//Hesite a mettre lseekEnd
	write(woody_fd, table, sizeof(t_table_haufman) * w->size_t_haufman);
	//munmap(w->elf.file_map, w->elf.file_size);
	printf("Nbr_tableHAufman %d", w->size_t_haufman);
	//MIse en place la fin de l'elf 
	
	close(woody_fd);
	return (0);
}
