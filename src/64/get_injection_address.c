#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>
#include <stdlib.h>

uint64_t try_PT_LOAD_address(t_elf64 *e, unsigned char *code, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t injection_address = 0;
	int i;

	for (i = 0; !injection_address && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD_and_PF_X(&Phdr[i]) && enough_zero_padding_in_segment(&Phdr[i], size))
		{
			injection_address = Phdr[i].p_vaddr + Phdr[i].p_filesz;
			// injection_address = Phdr[i].p_offset + Phdr[i].p_filesz;
		}
	}
	return (injection_address);
}

uint64_t try_PT_NOTE_address(t_elf64 *e, unsigned char *code, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t injection_address = 0;
	int i;

	for (i = 0; !injection_address && i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_NOTE(&Phdr[i]))
		{
			printf("PT_NOTE address\n");
			injection_address = Phdr[i].p_vaddr;
		}
	}
	return (injection_address);
}

uint64_t get_injection_offset(t_elf64 *e, unsigned char *code, int size)
{
	uint64_t injection_offset;

	injection_offset = try_PT_LOAD_address(e, code, size);
	if (injection_offset)
		return (injection_offset);
	
	injection_offset = try_PT_NOTE_address(e, code, size);
	return (injection_offset);
}
