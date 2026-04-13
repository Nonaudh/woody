
#include "woody64.h"

unsigned char	*get_section_by_header_64_unsigned_char(t_woody *e, Elf64_Shdr *sectionHeader)
{
	if (sectionHeader->sh_offset + sectionHeader->sh_size > (unsigned long)e->elf.file_size)
			return (NULL);

	unsigned char *ptr = e->elf.file_map + sectionHeader->sh_offset;
	return (ptr);
}

int init_table_haufman(t_haufman ***stack, Elf64_Shdr	*textHeader, unsigned char *test)
{
	int nbr_test = ft_nbr_type(test, 0, (*stack), 0, textHeader->sh_size);
	(*stack) = malloc(sizeof(t_haufman*) * (nbr_test + 1));//Mettre la securite
	for (int i = 0; i < nbr_test + 1	; i++)
	{
		(*stack)[i] = malloc(sizeof(t_haufman) * (nbr_test + 1));
		if (!(*stack)[i])
		return (1);
		ft_memset((*stack)[i], 0, sizeof(t_haufman) * (nbr_test + 1));
	}
	return(nbr_test);
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

int init_type_and_occurence_and_three_haufman(t_haufman **stack, Elf64_Shdr *textHeader, unsigned char *test, int nbr_test)
{
	ft_nbr_type(test, 1, stack, 0, textHeader->sh_size);//Implement les types	
	ft_nbr_type(test, 2, stack, nbr_test, textHeader->sh_size);//Implementes les occurences de types
	ft_memcpy(stack[0 + 1], stack[0],(nbr_test + 1)* sizeof(t_haufman)); //Hesite a mettre le -1
	three_haufman(stack,nbr_test ,1);
	return(0);
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

int implement_table_haufman(t_woody *e, t_haufman  **stack, t_table_haufman **table_codage)
{
	e->textHeader= get_section_header_by_name_64(e, "text");
	e->text_test = get_section_by_header_64_unsigned_char(e, e->textHeader);
	e->size_t_haufman = init_table_haufman(&stack, e->textHeader, e->text_test);
	if(e->size_t_haufman == 1)
	{
		return(1);
	}
	if(init_type_and_occurence_and_three_haufman(stack, e->textHeader, e->text_test, e->size_t_haufman) == 1)
		return(1);	
	int iterateur_table_codage = 0;
	*table_codage = malloc(sizeof(t_table_haufman) * e->size_t_haufman);
	if (!*table_codage)
	{
		return (1);
	}
	implement_table(&stack[e->size_t_haufman][0], 0, 0, *table_codage, &iterateur_table_codage );
	print_all_table_haufman(*table_codage, e->size_t_haufman);
	return(0);
}