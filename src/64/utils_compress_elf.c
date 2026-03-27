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
    tabe->code = malloc(depth + 1);
    int j = 0;
    for (int i = depth - 1; i >= 0; i--)
    {
        tabe->code[j] = ((code >> i) & 1) + '0';
        j++;
    }
    tabe->code[j] = '\0';
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

char *decompresser(t_table_haufman *table_codage, char *crypter, int nbr_type)
{
	char *decrypter = ft_strdup("");
	char *save = ft_strdup("");
	char **verif = ft_split(crypter, ' ');
	char tmp[2];
	for (int i = 0; verif[i] != NULL; i++)
	{
		for (int k = 0; k < nbr_type; k++)
		{
			if(ft_strncmp(table_codage[k].code, verif[i], ft_strlen(verif[i])) == 0)
			{
				// printf("%02x ", table_codage[k].type );
				tmp[0] =  table_codage[k].type;
				tmp[1] = '\0';
				save = ft_strjoin(decrypter, tmp);
				free(decrypter);
				decrypter = save;
				break;
			}
		}
	}
	return (decrypter);
}

char *compresser(t_table_haufman *table_codage, unsigned char *test, int nbr_type, int long_max_test)
{
	char *save_crypter  = ft_strdup("");
	char *tmp;
	char *tmp1;
	for (int i = 0; i < long_max_test; i++)
	{
		for(int k = 0; k < nbr_type ; k++)
		{
			if(table_codage[k].type  == test[i])
			{
				tmp1 = ft_strjoin(save_crypter," ");
                tmp = ft_strjoin(tmp1, table_codage[k].code);
                free(save_crypter);
                save_crypter = tmp;
                break;				
			}
		}		
	}
	return(save_crypter);
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

char * init_and_implement_codage_table(t_haufman **stack, Elf64_Shdr *textHeader, unsigned char *test, int nbr_test, t_table_haufman *table_codage)
{
	int iterateur_table_codage = 0;
	implement_table(&stack[nbr_test][0], 0, 0, table_codage, &iterateur_table_codage );
	return(compresser(table_codage, test, nbr_test, textHeader->sh_size));
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
	char *crypter = init_and_implement_codage_table(stack, textHeader, test, nbr_test, table_codage);
	char  *decrypter=decompresser(table_codage, crypter, nbr_test);
	return (0);
}