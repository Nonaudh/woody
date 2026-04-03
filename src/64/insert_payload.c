#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>
#include <stdlib.h>

int	segment_is_PT_LOAD_and_PF_X(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_LOAD && segment->p_flags & PF_X);
}

int	segment_is_PT_NOTE(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_NOTE);
}

int	enough_zero_padding_in_segment(Elf64_Phdr *segment, int size) // to check
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;

	if (padding > (int64_t)size)
		return (1);
	return (0);
}

void patch_marker(unsigned char *buf, size_t size, uint64_t marker, uint64_t value)
{
	printf("patch %lu to %lu\n", marker, value);
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

uint64_t	insert_in_PT_LOAD(t_elf64 *e, Elf64_Phdr *segment, unsigned char *code, int size)
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;
	uint64_t injection_adress = segment->p_vaddr + segment->p_filesz;

	segment->p_filesz += size;
	segment->p_memsz  += size;

	// patch_marker(code, size, 0xDEADDEADDEADDEAD, injection_adress);
	
	ft_memcpy((char *)e->file_map + fin_segment, code, size);
	
	return (injection_adress);
}

uint64_t	insert_in_PT_NOTE(t_elf64 *e, Elf64_Phdr *segment, unsigned char *code, int size)
{
	segment->p_type = PT_LOAD;
	segment->p_flags = PF_R | PF_X;

	segment->p_filesz += size;	
	segment->p_memsz  += size;

	// patch_marker(code, size, 0xDEADDEADDEADDEAD, segment->p_offset);
	
	ft_memcpy((char *)e->file_map + segment->p_offset, code, size);

	return (segment->p_offset);
}

uint64_t try_PT_LOAD(t_elf64 *e, unsigned char *code, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t new_entry_point = 0;
	int i;

	for (i = 0; !new_entry_point && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD_and_PF_X(&Phdr[i]) && enough_zero_padding_in_segment(&Phdr[i], size))
		{
			new_entry_point = insert_in_PT_LOAD(e, &Phdr[i], code, size);
		}
	}
	return (new_entry_point);
}

uint64_t try_PT_NOTE(t_elf64 *e, unsigned char *code, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t new_entry_point = 0;
	int i;

	for (i = 0; !new_entry_point && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_NOTE(&Phdr[i]))
		{
			printf("PT_NOTE injection\n");
			new_entry_point = insert_in_PT_NOTE(e, &Phdr[i], code, size);
		}
	}
	return (new_entry_point);
}

uint64_t insert_something_in_elf(t_elf64 *e, unsigned char *code, int size)
{
	uint64_t injection_adress;

	injection_adress = try_PT_LOAD(e, code, size);
	if (injection_adress)
		return (injection_adress);
	
	injection_adress = try_PT_NOTE(e, code, size);
	return (injection_adress);
}

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

unsigned char *read_payload(t_elf64 *e)
{
	int fd = open("payload", O_RDONLY);
	if (fd == -1)
		return (NULL);

	e->payload_size = lseek(fd, 0, SEEK_END) / 2;
	lseek(fd, 0, SEEK_SET);
	
	unsigned char *payload = malloc(e->payload_size);
	ft_read(fd, payload, e->payload_size);
	close (fd);

	return (payload);
}

void	patch_payload(t_elf64 *e, unsigned char *payload)
{
	patch_marker(payload, e->payload_size, 0xDEADBEEFCAFEBABE, e->elf_header->e_entry);

	uint64_t key_vaddr = insert_something_in_elf(e, e->key, e->key_size);
	uint64_t payload_injection_offset = get_injection_offset(e, payload, e->payload_size);

	patch_marker(payload, e->payload_size, 0xDEADDEADDEADDEAD, payload_injection_offset);
	patch_marker(payload, e->payload_size, 0xCAFECAFECAFECAFE, e->text_vaddr);
	patch_marker(payload, e->payload_size, 0xBABEBABEBABEBABE, e->text_size);
	patch_marker(payload, e->payload_size, 0xBEEFBEEFBEEFBEEF, key_vaddr);
	patch_marker(payload, e->payload_size, 0x4242424242424242, e->key_size);
}

uint64_t  insert_payload(t_elf64 *e)
{
	unsigned char *payload = read_payload(e);
	if (!payload)
	{
		printf("no Paypay\n");
		return (0);
	}

	patch_payload(e, payload);
	
	uint64_t new_entry_point = insert_something_in_elf(e, payload, e->payload_size);

	free(payload);

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
