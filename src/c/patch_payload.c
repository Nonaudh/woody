#include "woody64.h"

void patch_marker(t_woody *w, uint64_t marker, uint64_t value)
{
	unsigned char *buf = w->payload.payload;
	size_t size = w->payload.payload_size + w->payload.key_size;

    for (size_t i = 0; i < size; i++)
    {
        uint64_t *ptr = (uint64_t *)(buf + i);

        if (*ptr == marker)
        {
			printf("patch %lu to %lu\n", marker, value);
            *ptr = value;
            break ; 
        }
    }
}

int patch_payload(t_woody *w)
{
	printf("injection: offset; %lu  vaddr; %lu\n", w->injection.injection_offset, w->injection.injection_vaddr);
	w->injection.old_entry_point = w->elf.elf_header->e_entry;
	
	if (w->elf.elf_header->e_type == ET_EXEC)
	{
		printf("NOPIE\n");
		w->elf.elf_header->e_entry = w->injection.injection_vaddr;
		patch_marker(w, 0x6969696969696969, 1);
		patch_marker(w, 0xDEADDEADDEADDEAD, w->injection.injection_offset);
		patch_marker(w, 0xCAFECAFECAFECAFE, w->injection.old_entry_point);
		patch_marker(w, 0xBABEBABEBABEBABE, w->injection.xored_vaddr);
		patch_marker(w, 0xBEEFBEEFBEEFBEEF, w->injection.xored_size);
		patch_marker(w, 0xFEEDFEEDFEEDFEED, w->injection.injection_vaddr + w->payload.payload_size);
		patch_marker(w, 0x4242424242424242, w->payload.key_size);
	}
	else if (w->elf.elf_header->e_type == ET_DYN)
	{
		printf("PIE\n");
		w->elf.elf_header->e_entry = w->injection.injection_offset;
		patch_marker(w, 0x6969696969696969, 0);
		patch_marker(w, 0xDEADDEADDEADDEAD, w->injection.injection_vaddr);
		patch_marker(w, 0xCAFECAFECAFECAFE, w->injection.old_entry_point);
		patch_marker(w, 0xBABEBABEBABEBABE, w->injection.xored_vaddr);
		patch_marker(w, 0xBEEFBEEFBEEFBEEF, w->injection.xored_size);
		patch_marker(w, 0xFEEDFEEDFEEDFEED, w->injection.injection_vaddr + w->payload.payload_size);
		patch_marker(w, 0x4242424242424242, w->payload.key_size);	
	}
	return (0);
}
