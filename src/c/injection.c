#include "woody64.h"

int	actualize_segment(t_woody *w)
{
	if (w->injection.segment->p_type == PT_LOAD)
	{
		w->injection.segment->p_flags = PF_W | PF_X;
		w->injection.segment->p_filesz += w->payload.payload_size + w->payload.key_size;
		w->injection.segment->p_memsz += w->payload.payload_size + w->payload.key_size;
	}
	else if (w->injection.segment->p_type == PT_NOTE)
	{
    w->injection.segment->p_type   = PT_LOAD;
    w->injection.segment->p_flags  = PF_R | PF_X;
    w->injection.segment->p_offset = w->injection.injection_offset;  // offset fichier du payload
    w->injection.segment->p_vaddr  = w->injection.injection_vaddr;   // vaddr du payload
    w->injection.segment->p_paddr  = w->injection.injection_vaddr;   // idem
	w->injection.injection_vaddr% 0x1000 == w->injection.injection_offset % 0x1000;
    w->injection.segment->p_filesz = w->payload.payload_size + w->payload.key_size;
    w->injection.segment->p_memsz  = w->payload.payload_size + w->payload.key_size;
    w->injection.segment->p_align  = 0x1000;
	}
	return (0);
}

int injection (t_woody *w)
{
	printf("Size insert payload %d, size key payload %d", w->payload.payload_size ,  w->payload.key_size);
	ft_memcpy(w->elf.file_map + w->injection.injection_offset, w->payload.payload, w->payload.payload_size + w->payload.key_size);
	actualize_segment(w);
	return (0);
}
