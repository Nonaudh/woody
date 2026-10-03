#include "woody64.h"
#include <stdint.h>
#include <unistd.h>


void implement_one_bits(t_table_haufman *table_codage,  int nbr_test, unsigned char *test, int len_tt1, uint16_t *save1, int *k, int *nbr_len)
{
	// printf("Je suis nbrtest %d test = %02x  k = %d , nbr_len = %d\n",nbr_test, test, *k , *nbr_len);
	int i ;
	// for (int he = 0; he < 7 ; he++)
	// {
	// 	printf("")
	// }
	for(int j = 0; j < nbr_test; j++)
	{
		if (test[len_tt1] == table_codage[j].type)
		{
				if ((*nbr_len) == 0)
			{
				(*nbr_len) = table_codage[j].len;
				i = table_codage[j].len - 1; 
			}
			else
			{
				i = (*nbr_len) - 1;          
			}
			while(i >= 0)
			{
				int bit = (table_codage[j].code >> i) & 1;  
				(*save1) = ((*save1) << 1) | bit;           
				(*nbr_len)--;
				(*k)++;
				if((*k) == 16)
				{
					(*nbr_len) = i;
					return; 
				}
				if(i== 0)
				{
					(*nbr_len) = i;
						return; 
				}
				i--;
			}
			return;
		}
	}
}
void search_to_hufman(int k , unsigned char *all_elf, uint16_t *bit_hufman, int key_to_hufman, t_table_haufman *table_codage, t_woody *e)
{
	printf("\nSearch to %02x, in table haufman nbr boucle %d\n", all_elf[k], e->nbr_boucle_haufman);
	for (int i= 0 ; i < e->size_t_haufman; i++)
	{
		if(table_codage[i].type == all_elf[k])
		{
			printf("\n Find %02x  len = %d table_code= !", table_codage[i].type, table_codage[i].len);
			for (int b = table_codage[i].len - 1; b >= 0; b--)
						printf("%d", (table_codage[i].code >> b) & 1);
			printf("|\n");
		}
	}
}
int encryption_text(t_table_haufman *table_codage, t_woody *e, unsigned char *test, int nbr_test)//Pas encore ecrit a la place , de .text
{
	//MIse en place a des uin16_t en passant par des unsigned char 
	e->elf.all_elf_to_hufman_encrypte = (uint16_t*)e->elf.file_map;
	int key_to_hufman = 0;
	int k = 0;
 	// for(int k = 0; k <e->elf.file_size; k++)
 	// {
		printf("Search_to_hufman , %02x", e->elf.all_elf[0]);
		search_to_hufman(k, e->elf.all_elf, e->elf.all_elf_to_hufman_encrypte, key_to_hufman, table_codage, e);
 	// }
	// if(bits_safe != 0)
	// {
	// 	printf("\nnInclude to uint16_t\n");
	// 	return(0);
	// }
	return(0);
}


// uint16_t change_to_bits(uint16_t dest, uint16_t src, int i)
// {
// 	int bits = 0;
// 	int bits1 = 0;
// 	uint16_t essai ;
// 	for (int k = 14 ; k >= 0; k -- 	) 
// 	{
// 		bits = (src >>   k) & 1;
// 		essai =  (essai << 1) | bits;
// 	}
// 	return(essai);
// 	// return(change_to_bits( dest, src, i + 1));
// }


// int encryption_text(t_table_haufman *table_codage, t_woody *e, unsigned char *test, int nbr_test)//Pas encore ecrit a la place de .text
// {
// 	//Gros probleme sur le dest
// 	int pourreturn = 0; 
// 	printf("\nBefore\n");//Faire attention on passe par 8 bits 000000000
// 	int len_tt1 = 0;//Iterater sur .text
// 	int len__tt2 = 0;
// 	int k  = 0;//
// 	uint16_t save1 = 0;
// 	int nbr_len = 0;
// 	uint16_t *dest = (uint16_t *)e->elf.file_map; //La taille est trop grosse il vas falloir modifier ici 
// 	uint16_t *e->elf.all_elf_to_hufman_encrypte = (uint16_t*)e->elf.file_map;
// 	unsigned char *original = malloc(e->elf.file_size);
// 	if (!original)
// 		return (-1);
// 	ft_memcpy(original, dest, 0);
// 	test = original; 
// 	int dest_offset = 0;
// 	printf("\nIci\n");
// 	for(int k = 0; k <e->elf.file_size; k++)
// 	{
// 		printf("%02x", e->elf.all_elf[k]);
// 	}
// 	printf("\n");
// 	k = 0;
// 	int t5 = 0;
// 	printf("\nJe suis nbr_len=%d\n", nbr_len);
// 	while(len_tt1 <e->elf.file_size)//Tant que len__tt1 != 8
// 	{
// 		implement_one_bits(table_codage,   nbr_test,   e->elf.all_elf,  len_tt1,  &save1,  &k, &nbr_len);
// 		if (k == 16)
// 		{
// 			pourreturn++;
// 			dest[dest_offset] = (uint16_t)((save1 >> 16) & 1);
// 			if(dest_offset == 0)
// 			{
// 				printf("key of tabe = %d", dest_offset);
// 				printf("\nOne start save n");
// 				for (int i = 0; i < 15; i++)
// 					printf("%d", (save1 >> i) & 1);
// 				printf("\n");
// 				printf("\n One Start  dest n");
// 				for (int i = 0; i < 15; i++)
// 					printf("%d", (dest[dest_offset] >> i) & 1);
// 				printf("\n");
				
				
// 				printf("\n One Start  dest1 n");
// 				dest[dest_offset] = change_to_bits(dest[dest_offset], save1, 0);
// 				for (int i = 0; i < 15; i++)
// 					printf("%d", (dest[dest_offset] >> i) & 1);
// 				to_hufman(dest[dest_offsett], e->elf.all_elf_to_hufman_encrypte);
// 			}
// 			dest_offset++;	
// 			save1 = 0;                   
// 			k = 0;                       
// 		}
// 		if(nbr_len == 0)
// 		{
// 			len_tt1++;
// 			nbr_len = 0 ;
// 		}
// 	}
// 	while(len_tt1 <e->elf.file_size)
// 	{
// 		implement_one_bits(table_codage, nbr_test, e->elf.all_elf, len_tt1, &save1, &k, &nbr_len);
// 		if(nbr_len == 0)
// 			len_tt1++;
// 		if(k == 16)
// 		{
// 			dest[dest_offset++] = (uint16_t)((save1 >> 16) & 0xFF);
// 			dest[dest_offset++] = (uint16_t)(save1 & 0xFF);
// 			printf("dest = %02x save = %02x", dest[dest_offset], save1);
// 			save1 = 0;
// 			k = 0;
// 		}
// 	}
// 	if (k > 0)
// 	{
// 		save1 = save1 << (16 - k);
// 		dest[dest_offset++] = (uint16_t)((save1 >> 16) & 0xFF);
// 		if (k > 16)
// 		{
// 			dest[dest_offset++] = (uint16_t)(save1 & 0xFF);
// 		}
// 	}
// 	printf("Compresser e->elf.file_size=%d\n", e->elf.file_size);
// 	// ft_memset(&e->size_t_haufman, 0, e->size_t_haufman);//Revoir si ca peut etre utile 
// 	printf("/n Start encryption  nbr occurence dans l'encrytpion  %d\n ", dest_offset);
// 	for(int k = 0; k <dest_offset; k++) //IL faudrat le remettre pour permettre la verification de l'encryption	
// 	{
// 		printf("%02x", dest[k]);
// 	}
// 	printf("/n End encryption \n ");
// 	return(dest_offset);
// }