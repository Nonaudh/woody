#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>

int	segment_is_PT_LOAD(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_LOAD && segment->p_flags & PF_X);
}

int	segment_is_PT_NOTE(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_NOTE);
}

int	enough_zero_padding_in_segment(Elf64_Phdr *segment, int size)
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;

	if (padding > (int64_t)size)
		return (1);
	return (0);
}

void patch_marker(char *buf, size_t size, uint64_t marker, uint64_t value)
{
    for (size_t i = 0; i < size - 8; i++)
    {
        uint64_t *ptr = (uint64_t *)(buf + i);

        if (*ptr == marker)
        {
            *ptr = value;
            return;
        }
    }
}

uint64_t	insert_in_PT_LOAD(t_elf64 *e, Elf64_Phdr *segment, char *code, int size)
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;
	uint64_t injection_adress = segment->p_vaddr + segment->p_filesz;

	segment->p_filesz += size;
	segment->p_memsz  += size;
	
	ft_memcpy((char *)e->file_map + fin_segment, code, size);

	return (injection_adress);
}

uint64_t	insert_in_PT_NOTE(t_elf64 *e, Elf64_Phdr *segment, char *code, int size)
{
	segment->p_type = PT_LOAD;
	segment->p_flags = PF_R | PF_X;

	segment->p_filesz += size;	
	segment->p_memsz  += size;
	
	ft_memcpy((char *)e->file_map + segment->p_offset, code, size);

	return (segment->p_vaddr);
}

uint64_t try_PT_LOAD(t_elf64 *e, char *code, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t new_entry_point = 0;
	int i;

	for (i = 0; !new_entry_point && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD(&Phdr[i]) && enough_zero_padding_in_segment(&Phdr[i], size))
		{
			new_entry_point = insert_in_PT_LOAD(e, &Phdr[i], code, size);
		}
	}
	return (new_entry_point);
}

uint64_t try_PT_NOTE(t_elf64 *e, char *code, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t new_entry_point = 0;
	int i;

	for (i = 0; !new_entry_point && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_NOTE(&Phdr[i]))
		{
			new_entry_point = insert_in_PT_NOTE(e, &Phdr[i], code, size);
		}
	}
	return (new_entry_point);
}

uint64_t insert_something_in_elf(t_elf64 *e, char *code, int size)
{
	uint64_t injection_adress;

	injection_adress = try_PT_LOAD(e, code, size);
	if (injection_adress)
		return (injection_adress);
	
	injection_adress = try_PT_NOTE(e, code, size);
	return (injection_adress);
}

int count_hex(char *payload, int size)
{
	int i;
	int hex_size = 0;
	for (i = 0; i < size; i++)
	{
		if (payload[i] == '\\')
			hex_size++;
	}
	return (hex_size);
}

char *read_payload(t_elf64 *e)
{
	int fd = open("payload", O_RDONLY);
	if (fd == -1)
		return (NULL);

	e->payload_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	char *payload = malloc(e->payload_size);
	read(fd, payload, e->payload_size);

	e->payload_size = count_hex(payload, e->payload_size);


	printf("paysize %d\n", e->payload_size);
	printf("payload; %s\n", payload);

	return (payload);
}

uint64_t  insert_payload(t_elf64 *e)
{
	char payloadtmp[] = "\x31\xc0\x99\xb2\x0a\xff\xc0\x89\xc7\x48\x8d\x35\x12\x00\x00\x00\x0f\x05"
    "\xb2\x2a\x31\xc0\xff\xc0\xf6\xe2\x89\xc7\x31\xc0\xb0\x3c\x0f\x05\x2e\x2e"
    "\x57\x4f\x4f\x44\x59\x2e\x2e\x0a";
	e->payload_size = sizeof(payloadtmp) - 1;
	printf("paysize %d\n", e->payload_size);


	char *payload = read_payload(e);
	if (!payload)
	{
		printf("no Paypay\n");
		return (0);
	}

	// char payload[] = "\x4d\x31\xc0\x4d\x31\xc9\x49\xba\xbe\xba\xfe\xca\xef\xbe\xad\xde\x41\xff\xe2";

	// printf("%lu\n", e->entry_point);
	// patch_marker(payload, e->payload_size, 0xDEADBEEFCAFEBABE, e->entry_point);

	uint64_t new_entry_point = insert_something_in_elf(e, payload, e->payload_size);

	// free(payload);

	return (new_entry_point);
}

int injection(t_elf64 *e)
{
	uint64_t	new_entry_point;

	new_entry_point = insert_payload(e);
	if (!new_entry_point)
		return (1);
	ft_memcpy((char *)e->file_map  + 0x18, &new_entry_point, 8);

	return (0);
}
