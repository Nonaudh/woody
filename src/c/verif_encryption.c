#include "woody64.h"

int search_current(uint16_t current, t_table_haufman *table_codage, int nbr_table_codage,unsigned  char *dest_tab, int *e, int len_current)
{
	int bits  = 0;
	uint16_t save =0; 
	for(int j = 0; j < nbr_table_codage; j++)
	{
		for (int r = table_codage[j].len - 1; r >= 0; r--) {
			bits = (table_codage[j].code >> r) & 1;
			save = (save << 1) | bits; // cumule tous les bits
		}	
		if(current == table_codage[j].code && len_current == table_codage[j].len)
		{
				dest_tab[(*e)] = table_codage[j].type;
				(*e)++;
				return(1);
		}
	}
	return(0);
}

void verif_encrytpion(t_table_haufman *table_codage, t_elf *e, Elf64_Shdr *textHeader, unsigned char *test, int nbr_table_codage, int nbr_fort)//Y a moyens d'enlever le section header
{
	printf("Verif_encryption\n");
	uint8_t *dest = (uint8_t *)e->file_map + textHeader->sh_offset;
	int iterateur_dest = 0;
	int bits = 0;
	unsigned char dest_tab[textHeader->sh_size + 1];// A verifier
	int len_current = 0;
	int occurence_tab = 0;
	int j = 0;
	uint16_t current = 0;
	for(int k = 0;k < nbr_fort  ; k++ )
	{
		for ( j = 7; j >= 0; j--)
		{ 
			len_current ++;
			bits = ((dest[k] >> j) & 1);
			current = (current << 1) | bits;
			if(search_current(current, table_codage, nbr_table_codage, dest_tab, &occurence_tab, len_current)==1)
			{
				current = 0;
				len_current = 0;
				bits = 0;
			}
		}
	}	
	dest_tab[occurence_tab] = '\0';
	printf("\n");
	ft_memcpy(dest, dest_tab, textHeader->sh_size);
	for (int i = 0; i < textHeader->sh_size ; i++)
		printf("%02x", dest[i]);
	printf("\n");
}