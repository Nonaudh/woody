#include "woody64.h"

void patch_marker(unsigned char *buf, size_t size, uint64_t marker, uint64_t value)
{
	printf("patch %lu to %lu\n", marker, value);
    for (size_t i = 0; i < size - 8; i++)
    {
        uint64_t *ptr = (uint64_t *)(buf + i);

        if (*ptr == marker)
        {
            *ptr = value;
            break ; 
        }
    }
}

int patch_payload(t_woody *w)
{
	w->injection.old_entry_point = w->elf.elf_header->e_entry;
	w->elf.elf_header->e_entry = w->injection.injection_offset;
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xDEADDEADDEADDEAD, w->injection.injection_vaddr);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xCAFECAFECAFECAFE, w->injection.old_entry_point);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xBABEBABEBABEBABE, w->injection.text_vaddr);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xBEEFBEEFBEEFBEEF, w->injection.text_size);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xFEEDFEEDFEEDFEED, w->injection.injection_vaddr + w->payload.payload_size);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0x4242424242424242, w->payload.key_size);	
	return (0);
}
