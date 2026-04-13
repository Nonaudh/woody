#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>
#include <stdlib.h>

unsigned char str_to_hexa_char(char nb[3])
{
	unsigned char hex = 0;

	if (ft_strlen(nb) != 2)
		return (0);
	
	if (nb[0] >= 'a' && nb[0] <= 'f')
		nb[0] -= 87;
	else
		nb[0] -= 48;
	hex += (nb[0] * 16);
	if (nb[1] >= 'a' && nb[1] <= 'f')
		nb[1] -= 87;
	else
		nb[1] -= 48;
	hex += nb[1];

	return (hex);
}

void	ft_read(int fd, unsigned char *payload, int size)
{
	char nb[3];
	unsigned char dec;

	nb[2] = 0;

	for (int i = 0; i < size; i++)
	{	
		read(fd, nb, 2);
		dec = str_to_hexa_char(nb);
		payload[i] = dec;
	}
}

unsigned char *key_gen(t_woody *w)
{
	unsigned char *key;

	if (w->payload.payload)
	{
		w->payload.key_size = ft_strlen((char *)w->payload.payload);
		return ((unsigned char *)ft_strdup((char *)w->payload.payload));
	}
	else
	{
		int fd = open("/dev/urandom", O_RDONLY);
		if (fd == -1)
			return (NULL);
		w->payload.key_size = 10;
		key = malloc(sizeof(unsigned char) * w->payload.key_size);
		if (!key)
		{
			close (fd);
			return (NULL);
		}
		if (read(fd, key, w->payload.key_size) != w->payload.key_size)
		{
			close (fd);
			return (NULL);
		}
		close (fd);
	}
	return (key);
}

int read_payload(t_woody *w)
{
	int fd = open("payload", O_RDONLY);
	if (fd == -1)
	{
		perror("payload");
		return (1);
	}
	
	unsigned char *key = key_gen(w);
	if (!key)
	{
		close (fd);
		return (1);
	}

	w->payload.payload_size = lseek(fd, 0, SEEK_END) / 2;
	lseek(fd, 0, SEEK_SET);
	
	w->payload.payload = malloc(w->payload.payload_size + w->payload.key_size);
	if (!w->payload.payload)
	{
		free (key);
		close (fd);
		return (1);
	}
	ft_read(fd, w->payload.payload, w->payload.payload_size);
	ft_memcpy(w->payload.payload + w->payload.payload_size, key, w->payload.key_size);
	free (key);
	close (fd);

	return (0);
}
