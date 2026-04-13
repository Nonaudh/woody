#include "woody64.h"


void implement_one_bits(t_table_haufman *table_codage,  int nbr_test, unsigned char *test, int len_tt1, uint16_t *save1, int *k, int *nbr_len)
{
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


int encryption_text(t_table_haufman *table_codage, t_woody *e, Elf64_Shdr *textHeader, unsigned char *test, int nbr_test)//Pas encore ecrit a la place de .text
{
	int pourreturn = 0; 
	printf("\nBefore\n");
	int len_tt1 = 0;//Iterater sur .text
	int len__tt2 = 0;
	int k  = 0;//
	uint16_t save1 = 0;
	int nbr_len = 0;
	uint8_t *dest = (uint8_t *)e->elf.file_map + textHeader->sh_offset;

	unsigned char *original = malloc(textHeader->sh_size);
	if (!original)
		return (-1);
	ft_memcpy(original, dest, textHeader->sh_size);
	test = original; 
	int dest_offset = 0;
	for(int k = 0; k <textHeader->sh_size; k++)
	{
		printf("%02x", dest[k]);
	}
	printf("\n");
	k = 0;
	int t5 = 0;
	while(len_tt1 < (int)textHeader->sh_size )//Tant que len__tt1 != 8
	{
		implement_one_bits(table_codage,   nbr_test,   test,  len_tt1,  &save1,  &k, &nbr_len);
		
		if (k == 16)
		{
			pourreturn++;
			dest[dest_offset] = (uint8_t)((save1 >> 8) & 0xFF);
			dest_offset++;
			dest[dest_offset] = (uint8_t)(save1 & 0xFF);    
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
	while(len_tt1 < (int)textHeader->sh_size)
	{
		implement_one_bits(table_codage, nbr_test, test, len_tt1, &save1, &k, &nbr_len);
		if(nbr_len == 0)
			len_tt1++;
		if(k == 16)
		{
			dest[dest_offset++] = (uint8_t)((save1 >> 8) & 0xFF);
			dest[dest_offset++] = (uint8_t)(save1 & 0xFF);
			save1 = 0;
			k = 0;
		}
	}
	if (k > 0)
	{
		save1 = save1 << (16 - k);
		dest[dest_offset++] = (uint8_t)((save1 >> 8) & 0xFF);
		if (k > 8)
			dest[dest_offset++] = (uint8_t)(save1 & 0xFF);
	}
	ft_memset(dest + dest_offset, 0, textHeader->sh_size - dest_offset);
	printf("Compresser\n");
	for(int k = 0; k <textHeader->sh_size; k++)
	{
		printf("%02x", dest[k]);
	}
	return(dest_offset);
}