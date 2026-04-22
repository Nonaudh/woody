#include "woody.h"

void xor_xor(char *text, int size, unsigned char *key, int key_size)
{
	int j = 0;

	for (int i = 0; i < size; i++)
	{
		if (j == key_size)
			j = 0;
		text[i] ^= key[j];
		j++;
	}
}

int get_pt_load(t_woody *w)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(w->elf.file_map + w->elf.elf_header->e_phoff);
	int found = 0;
	int i;

	for (i = 0; !found && i < w->elf.elf_header->e_phnum; i++)
	{
		if (segment_is_PT_LOAD_and_PF_X(&Phdr[i]) && Phdr[i].p_offset >= w->elf.elf_header->e_ehsize)
		{
			w->injection.xored_vaddr = Phdr[i].p_vaddr;
			w->injection.xored_size = Phdr[i].p_filesz;
			w->injection.xored_offset = Phdr[i].p_offset;
			found = 1;
		}
	}
	if (found)
		return (0);
	ft_dprintf(2, "Can't found any segment to xor\n");
	return (1);
}

int	xor_pt_load(t_woody *w)
{
	if (get_pt_load(w))
		return (1);
	xor_xor(w->elf.file_map + w->injection.xored_offset, w->injection.xored_size, w->payload.payload + w->payload.payload_size, w->payload.key_size);
	return (0);
}
