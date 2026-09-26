#include "woody64.h"
#include <unistd.h>


void implement_one_bits(t_table_haufman *table_codage,  int nbr_test, unsigned char *test, int len_tt1, uint16_t *save1, int *k, int *nbr_len)
{
	// printf("Je suis nbrtest %d test = %02x  k = %d , nbr_len = %d\n",nbr_test, test, *k , *nbr_len);
	int i ;
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


int encryption_text(t_table_haufman *table_codage, t_woody *e, unsigned char *test, int nbr_test)//Pas encore ecrit a la place de .text
{
	int pourreturn = 0; 
	printf("\nBefore\n");//Faire attention on passe par 8 bits 000000000
	int len_tt1 = 0;//Iterater sur .text
	int len__tt2 = 0;
	int k  = 0;//
	uint16_t save1 = 0;
	int nbr_len = 0;
	uint16_t *dest = (uint16_t *)e->elf.file_map;
	unsigned char *original = malloc(e->elf.file_size);
	if (!original)
		return (-1);
	ft_memcpy(original, dest, 0);
	test = original; 
	int dest_offset = 0;
	printf("\nIci\n");
	for(int k = 0; k <e->elf.file_size; k++)
	{
		printf("%02x", e->elf.all_elf[k]);
	}
	printf("\n");
	k = 0;
	int t5 = 0;
	printf("\nJe suis nbr_len=%d\n", nbr_len);
	while(len_tt1 <e->elf.file_size)//Tant que len__tt1 != 8
	{
		implement_one_bits(table_codage,   nbr_test,   e->elf.all_elf,  len_tt1,  &save1,  &k, &nbr_len);
		if (k == 16)
		{
			pourreturn++;
			dest[dest_offset] = (uint16_t)((save1 >> 16) & 0xFF);
			dest_offset++;
			dest[dest_offset] = (uint16_t)(save1 & 0xFF);    
			dest_offset++;	
			save1 = 0;                   
			k = 0;                       
		}
		if(nbr_len == 0)
		{
			len_tt1++;
			nbr_len = 0 ;
		}
	}
	while(len_tt1 <e->elf.file_size)
	{
		implement_one_bits(table_codage, nbr_test, e->elf.all_elf, len_tt1, &save1, &k, &nbr_len);
		if(nbr_len == 0)
			len_tt1++;
		if(k == 16)
		{
			dest[dest_offset++] = (uint16_t)((save1 >> 16) & 0xFF);
			dest[dest_offset++] = (uint16_t)(save1 & 0xFF);
			printf("dest = %02x save = %02x", dest[dest_offset], save1);
			save1 = 0;
			k = 0;
		}
	}
	if (k > 0)
	{
		save1 = save1 << (16 - k);
		dest[dest_offset++] = (uint16_t)((save1 >> 16) & 0xFF);
		if (k > 16)
		{
			dest[dest_offset++] = (uint16_t)(save1 & 0xFF);
		}
	}
	printf("Compresser e->elf.file_size=%d\n", e->elf.file_size);
	// ft_memset(&e->size_t_haufman, 0, e->size_t_haufman);//Revoir si ca peut etre utile 
	printf("/n Start encryption  nbr occurence dans l'encrytpion  %d\n ", dest_offset);
	for(int k = 0; k <dest_offset; k++) //IL faudrat le remettre pour permettre la verification de l'encryption	
	{
		printf("%02x", dest[k]);
	}
	printf("/n End encryption \n ");
	return(dest_offset);
}