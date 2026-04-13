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
	
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(w->elf.file_map + w->elf.elf_header->e_phoff);
	uint64_t save_max = 0;
	uint64_t tmp;
	for(int i = 0; i < w->elf.elf_header->e_phnum ; i++)
	{
		if (Phdr[i].p_type != PT_LOAD)
			continue;
		tmp = Phdr[i].p_vaddr + Phdr[i].p_memsz;
		if (tmp > save_max)
		{
			save_max = tmp;
		}
	}
	 printf("\nBytes haufman %zu\n", sizeof(t_table_haufman));
	printf("Je suis le max phdr %lu\n", save_max);
	w->injection.old_entry_point = w->elf.elf_header->e_entry;
	w->elf.elf_header->e_entry = w->injection.injection_offset;
	//Mise en place des 2 patch 
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0x4040404040404040, w->nbr_boucle_haufman);//Nbr de boucle a faire 
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0x4141414141414141, w->size_t_haufman);//Taille de la tablea de haufman 
	//IL faut aussi patcher la fin de l'elf
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0x4343434343434343, save_max);


	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xDEADDEADDEADDEAD, w->injection.injection_vaddr);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xCAFECAFECAFECAFE, w->injection.old_entry_point);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xBABEBABEBABEBABE, w->injection.text_vaddr);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xBEEFBEEFBEEFBEEF, w->injection.text_size);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0xFEEDFEEDFEEDFEED, w->injection.injection_vaddr + w->payload.payload_size);
	patch_marker(w->payload.payload, w->payload.payload_size + w->payload.key_size, 0x4242424242424242, w->payload.key_size);	
	return (0);
}
