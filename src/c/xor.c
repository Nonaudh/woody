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

int	xor_point_text(t_woody *w)
{
	Elf64_Shdr *text_header = get_section_header_by_name_64(w, "text");
	if (!text_header)
		return (1);
	w->injection.text_vaddr = text_header->sh_addr;
	w->injection.text_size = text_header->sh_size;
	printf("text_vaddr; %lu  text_size; %d\n", w->injection.text_vaddr, w->injection.text_size);
	xor_xor(w->elf.file_map + text_header->sh_offset, text_header->sh_size, w->payload.payload + w->payload.payload_size, w->payload.key_size);
	return (0);
}
