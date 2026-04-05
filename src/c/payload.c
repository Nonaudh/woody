#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>
#include <stdlib.h>

void	ft_read(int fd, unsigned char *payload, int size)
{
	char nb[3];
	unsigned char dec;

	nb[2] = 0;

	for (int i = 0; i < size; i++)
	{	
		read(fd, nb, 2);
		dec = strtol(nb, NULL, 16); //to recode !
		payload[i] = dec;
		printf("%d ", dec);
	}
	printf("\n");
}

unsigned char *key_gen(t_woody *w)
{
	w->payload.key_size = 4;
	unsigned char *key = malloc(sizeof(unsigned char) * w->payload.key_size);
	if (!key)
		return (NULL);
	ft_memcpy(key, "AKEY", w->payload.key_size);

	return (key);
}

int read_payload(t_woody *w)
{
	int fd = open("payload", O_RDONLY);
	if (fd == -1)
		return (1);

	unsigned char *key = key_gen(w);
	if (!key)
		return (1);

	w->payload.payload_size = lseek(fd, 0, SEEK_END) / 2;
	lseek(fd, 0, SEEK_SET);
	
	w->payload.payload = malloc(w->payload.payload_size + w->payload.key_size);
	ft_read(fd, w->payload.payload, w->payload.payload_size);
	ft_memcpy(w->payload.payload + w->payload.payload_size, key, w->payload.key_size);
	close (fd);

	return (0);
}
