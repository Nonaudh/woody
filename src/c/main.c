#include "woody64.h"
#include <sys/mman.h>

int safe_exit(t_woody *w)
{
	if (w->elf.file_map)
		munmap(w->elf.file_map, w->elf.file_size);
	if (w->payload.payload)
		free(w->payload.payload);
	return (1);
}

int main(int argc, char **argv)
{
	t_woody w;
	t_haufman  **stack;
	t_table_haufman *table_codage;
	
	int save = 0; //A supprimer juste pour verif
	bzero_struct(&w);

	if (check_args(&w, argc, argv))
		return (1);
	
	if (init_elf_64(&w))
	{
		if (w.elf.file_map)
			munmap(w.elf.file_map, w.elf.file_size);
		return (1);
	}

	if(implement_table_haufman(&w, stack, &table_codage))
		return(1);
		
	if (read_payload(&w))
	{
		if (w.elf.file_map)
			munmap(w.elf.file_map, w.elf.file_size);
		return (1);
	}

	//Il faut mettre la compression avant le xor
	w.nbr_boucle_haufman = encryption_text(table_codage, &w, w.textHeader, w.text_test, w.size_t_haufman);
	
	//Verification mise en place de la desencryption 
	verif_encrytpion(table_codage, &w.elf ,w.textHeader, w.text_test, w.size_t_haufman, w.nbr_boucle_haufman);
	
	if (xor_pt_load(&w))
		return (safe_exit(&w));
	
	if (get_injection_offset(&w))
		return (safe_exit(&w));

	if (patch_payload(&w))
		return (safe_exit(&w));

	if (injection(&w))
		return (safe_exit(&w));

	if (copy_into_woody(&w, table_codage))
		return (safe_exit(&w));

	print_key(&w);
	safe_exit(&w);
	return (0);
}
