#include "woody.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>
#include <stdlib.h>

int	segment_is_PT_LOAD_and_PF_X(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_LOAD && segment->p_flags & PF_X);
}

int	enough_zero_padding_in_segment(Elf64_Phdr *segment, int size) // to check
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;

	if (padding > (uint64_t)size)
		return (1);
	return (0);
}

uint64_t try_PT_LOAD_offset(t_woody *w, int size)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(w->elf.file_map + w->elf.elf_header->e_phoff);
	int found = 0;

	int i;

	for (i = 0; !found && i < w->elf.elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD_and_PF_X(&Phdr[i]) && enough_zero_padding_in_segment(&Phdr[i], size))
		{
			w->injection.injection_vaddr = Phdr[i].p_vaddr + Phdr[i].p_filesz;
			w->injection.injection_offset = Phdr[i].p_offset + Phdr[i].p_filesz;
			w->injection.segment = &Phdr[i];
			found = 1;
		}
	}
	return (found);
}

int get_injection_offset(t_woody *w)
{
	if (try_PT_LOAD_offset(w, w->payload.payload_size + w->payload.key_size))
		return (0);
	ft_dprintf(2, "Not enough zero padding for injection\n");
	return (1);
}
