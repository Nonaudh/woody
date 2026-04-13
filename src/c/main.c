#include "woody64.h"

int main(int argc, char **argv)
{
	t_woody w;
	t_haufman  **stack;
	t_table_haufman *table_codage;
	
	int save = 0; //A supprimer juste pour verif
	if (argc != 2)
		return (1);

	if (check_file(argv[1]))
		return (1);
	
	if (init_elf_64(&w, argv[1]))
		return (1);

	if(implement_table_haufman(&w, stack, &table_codage))
		return(1);
		
	if (read_payload(&w))
		return (1);

	//Il faut mettre la compression avant le xor
	w.nbr_boucle_haufman = encryption_text(table_codage, &w, w.textHeader, w.text_test, w.size_t_haufman);
	
	//Verification mise en place de la desencryption 
	verif_encrytpion(table_codage, &w.elf ,w.textHeader, w.text_test, w.size_t_haufman, w.nbr_boucle_haufman);

	if (xor_point_text(&w))
		return (1);
	
	if (get_injection_offset(&w))
		return (1);

	if (patch_payload(&w))
		return (1);

	if (injection(&w))
		return (1);

	if (copy_into_woody(&w, table_codage))
		return (1);
}
