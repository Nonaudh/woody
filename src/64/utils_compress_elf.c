#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>

int search_in_case(t_haufman *stack, unsigned char search, int longueur_max)
{
	for(int i = 0; i < longueur_max; i++)
	{
		if(search == stack[i].type)
		{
			stack[i].nbr_occurence++;
			return(0);
		}
	}
	return(1);
}

int search_nbr_two(t_haufman *stack, int max, int save_one, int clef_one)
{
	for (int k = 0; k < max ; k++)
	{
		if(stack[k].nbr_occurence <= save_one && k != clef_one)
		{
			return(k);
		}
	}
	return(search_nbr_two(stack, max, save_one+1, clef_one));
}

void three_haufman(t_haufman **stack, int max , int x_hold)
{
	if(max ==1)
		return ;
	int save_one = 999999999;
	int save_clef =0;
	int save_two = 999999999;
	int j = 0;
	for(j = 0; j < max ; j++)
	{
		if(save_one >  stack[x_hold][j].nbr_occurence)
		{
			save_one =  stack[x_hold][j].nbr_occurence;
			save_clef = j;
		}
	}
	int search_two = search_nbr_two(stack[x_hold] ,max,  save_one , save_clef);
	t_haufman *node_one; 
	node_one = malloc(sizeof(t_haufman) * 1);
	*node_one = stack[x_hold][save_clef];//Je dois peux etre utiliser un ft_memcopy
	t_haufman *node_two; 
	node_two = malloc(sizeof(t_haufman) * 1);
	*node_two = stack[x_hold][search_two];//Je dois peux etre utiliser un 
	if(save_clef == max)
	{
		stack[x_hold][search_two].nbr_occurence = stack[x_hold][search_two].nbr_occurence + stack[x_hold][save_clef].nbr_occurence;
		stack[x_hold][search_two].right = node_two;
		stack[x_hold][search_two].left = node_one;
		stack[x_hold][search_two].type = ' ';
	}
	else
	{
		stack[x_hold][save_clef].nbr_occurence = node_one->nbr_occurence + node_two->nbr_occurence;
		stack[x_hold][save_clef].right = node_two;
		stack[x_hold][save_clef].left = node_one;
		stack[x_hold][save_clef].type = ' ';		
		stack[x_hold][search_two] = stack[x_hold][max - 1];
	}
	ft_memcpy(stack[x_hold + 1], stack[x_hold],(max )* sizeof(t_haufman)); //Hesite a mettre le -1
	max--;
	if (max != 0 )
		three_haufman(stack, max, x_hold + 1);
	return;
}

void implement_code_table(int code, int depth, t_table_haufman *tabe)
{
    tabe->len  = depth;
    int e = 0;
	int add = 0;
	char *save = malloc(17);
	
    for (int i = depth - 1; i >= 0; i--)
    {
		save[e] = ((code >> i) & 1) + '0';
        e++;
    }
	save[e]  = '\0';
	int savec = 0;
	int b = 0;
	while(save[b] != '\0')
	{
		savec = (savec <<1) | (save[b] - '0');
		b++;
	}
	tabe->code = savec;
}

void implement_table(t_haufman *noeuds, int step, int code, t_table_haufman *tabe_codage, int *iterateur_table)
{
    if (noeuds->left == NULL && noeuds->right == NULL)
    {
		tabe_codage[*iterateur_table].type = noeuds ->type;
        implement_code_table(code, step, &tabe_codage[*iterateur_table]);
		(*iterateur_table)++;
        return;
    }
	if(noeuds->left != NULL)
	{
		implement_table(noeuds->left, step + 1, code << 1, tabe_codage, iterateur_table);	
	}
	if(noeuds->right != NULL)
	{
		implement_table(noeuds->right, step + 1, (code << 1) | 1, tabe_codage, iterateur_table);
	}
}

int ft_nbr_type(unsigned char *test, int y , t_haufman **stack, int nbr_max, int limits)//Probablement un free ici 
{
	int nbr = 0;
	int nbr_table = 0;
	int j;
	
	int save = 0;
	unsigned char table[limits];
	table[0] = '\0';
	for(int i = 0; i < limits; i++)
	{ 
		for(j = 0; j < nbr; j++)
			if(test[i] == table[j])
				break;
		if (j == nbr)
		{	
			table[j] = test[i];
			table[j + 1] = '\0';
			nbr++;
		}
	}
	if(y == 1)
	{
			for(int k = 0; k<nbr; k++)
			{
				stack[0][k].type = table[k];
			}
	}
	if(y == 2)
	{
		for(int g = 0; g < limits;g++)
		{
			for(int w = 0; w < nbr_max ;w++)
			{
				if(test[g] == stack[0][w].type)	
				{
					stack[0][w].nbr_occurence++;					
				}
			}
		}
	}
	return(nbr);
}
							
int init_table_haufman(t_haufman ***stack, Elf64_Shdr	*textHeader, unsigned char *test)
{
	int nbr_test = ft_nbr_type(test, 0, (*stack), 0, textHeader->sh_size);//Calcule le nbr de type
	(*stack) = malloc(sizeof(t_haufman*) * (nbr_test + 1));//Vient d'enlver 100
	for (int i = 0; i < nbr_test + 1	; i++)
	{
		(*stack)[i] = malloc(sizeof(t_haufman) * (nbr_test + 1));
		if (!(*stack)[i])
		return (1);
		ft_memset((*stack)[i], 0, sizeof(t_haufman) * (nbr_test + 1));
	}
	return(nbr_test);
}

int init_type_and_occurence_and_three_haufman(t_haufman **stack, Elf64_Shdr *textHeader, unsigned char *test, int nbr_test)
{
	ft_nbr_type(test, 1, stack, 0, textHeader->sh_size);//Implement les types	
	ft_nbr_type(test, 2, stack, nbr_test, textHeader->sh_size);//Implementes les occurences de types
	int test_nbr_occurence = 0;
	ft_memcpy(stack[0 + 1], stack[0],(nbr_test + 1)* sizeof(t_haufman)); //Hesite a mettre le -1
	three_haufman(stack,nbr_test ,1);
	return(0);
}

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

int encryption_text(t_table_haufman *table_codage, t_elf64 *e, Elf64_Shdr *textHeader, unsigned char *test, int nbr_test)
{
	int pourreturn = 0; 
	printf("\nBefore\n");
	int len_tt1 = 0;//Iterater sur .text
	int len__tt2 = 0;
	int k  = 0;//
	uint16_t save1 = 0;
	int nbr_len = 0;
	uint8_t *dest = (uint8_t *)e->file_map + textHeader->sh_offset;

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

void verif_encrytpion(t_table_haufman *table_codage, t_elf64 *e, Elf64_Shdr *textHeader, unsigned char *test, int nbr_table_codage, int nbr_fort)//Y a moyens d'enlever le section header
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

void insert_to_elf_table(t_table_haufman *table_codage, t_elf64 *e, int nbr_table_codage)
{
    uint32_t table_count = nbr_table_codage;
    int      offset      = 0;
    uint64_t magic       = 0x4242424242424242;  // ancre

    char *inject_zone = (char *)e->file_map + e->file_size_pre_hand;

    // 1. Shellcode
    ft_memcpy(inject_zone + offset, e->shellcode, e->size_code);
    offset += e->size_code;

    // 2. Magic marker
    ft_memcpy(inject_zone + offset, &magic, sizeof(uint64_t));
    offset += sizeof(uint64_t);

    // 3. table_count
    ft_memcpy(inject_zone + offset, &table_count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // 4. La table
    ft_memcpy(inject_zone + offset, table_codage, sizeof(t_table_haufman) * nbr_table_codage);
}
void print_all_table_haufman(t_table_haufman *table, int nbr_fort)
{
    printf("=== TABLE HUFFMAN (%d entrees) ===\n", nbr_fort);
    for (int i = 0; i < nbr_fort; i++)
    {
        printf("[%2d] type=0x%02x ('%c') code=0x%04x len=%d  bits=",
            i,
            table[i].type,
            (table[i].type >= 32 && table[i].type < 127)
                ? table[i].type : '.',
            table[i].code,
            table[i].len);
        for (int b = table[i].len - 1; b >= 0; b--)
            printf("%d", (table[i].code >> b) & 1);
        printf("\n");
    }
    printf("=================================\n");
}

char * init_and_implement_codage_table(t_haufman **stack, Elf64_Shdr *textHeader, unsigned char *test, int nbr_test, t_table_haufman *table_codage,  t_elf64 *e)
{
	int iterateur_table_codage = 0;
	implement_table(&stack[nbr_test][0], 0, 0, table_codage, &iterateur_table_codage );
	
	int nbr_for = encryption_text(table_codage, e,textHeader, test, nbr_test);
	insert_to_elf_table(table_codage, e, nbr_test);
	print_all_table_haufman(table_codage, nbr_test);
	verif_encrytpion(table_codage, e, textHeader, test, nbr_test, nbr_for);
	return(NULL);
}

int	compression_decompression(t_elf64 *e)
{
	Elf64_Shdr	*textHeader = get_section_header_by_name_64(e, ".text");
	unsigned char	*test = get_section_by_header_64_unsigned_char(e, textHeader);
	// //Mise en place du test
	t_haufman  **stack;
	int nbr_test;
	nbr_test = init_table_haufman(&stack, textHeader, test);
	if(nbr_test == 1)
		return(1);
	if(init_type_and_occurence_and_three_haufman(stack, textHeader, test, nbr_test) == 1)
		return(1);	
	t_table_haufman table_codage[nbr_test];
	char *crypter = init_and_implement_codage_table(stack, textHeader, test, nbr_test, table_codage, e);
	return (0);
}