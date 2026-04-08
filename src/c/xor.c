#include "woody64.h"

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

Elf64_Phdr *get_pt_load(t_woody *w)
{
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(w->elf.file_map + w->elf.elf_header->e_phoff);
	int found = 0;
	int i;

	for (i = 0; !found && i < w->elf.elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD_and_PF_X(&Phdr[i]))
		{
			w->injection.xored_vaddr = Phdr[i].p_vaddr;
			w->injection.xored_size = Phdr[i].p_filesz;
			w->injection.xored_offset = Phdr[i].p_offset;
			found = 1;
		}
	}
	if (found)
		return (&Phdr[i]);
	return (NULL);
}



int	xor_pt_load(t_woody *w)
{
	// Elf64_Phdr *pt_load = get_pt_load(w);
	// if (!pt_load)
	// 	return (1);

	Elf64_Shdr *text = get_section_header_by_name_64(w, "text");
	if (!text)
		return (1);

	w->injection.xored_offset = text->sh_offset;
	w->injection.xored_vaddr = text->sh_addr;
	w->injection.xored_size = text->sh_size;

	printf("xored_vaddr; %lu  xored_size; %lu  xored_offset; %lu\n", w->injection.xored_vaddr, w->injection.xored_size,  w->injection.xored_offset);
	// xor_xor(w->elf.file_map + w->injection.xored_offset, w->injection.xored_size, w->payload.payload + w->payload.payload_size, w->payload.key_size);
	return (0);
}
