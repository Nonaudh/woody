#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>

long	size_of_file_64(int fd, char *filename)
{
	struct stat stat;

	if (fstat(fd, &stat))
		return (-1);
	if (stat.st_size < (long)sizeof(Elf64_Ehdr))
	{
		ft_dprintf(2, "nm: %s: file format not recognized\n", filename);
		return (-1);
	}
	return (stat.st_size);
}

int	init_elf_64(t_elf64 *e, char *filename)
{
	e->file_map = NULL;
	e->fd = open(filename, O_RDWR);
	if (e->fd == -1)
	{
		perror(filename);
		return (1);
	}
	e->filename = filename;
	e->file_map = get_file_in_a_map_write_64(e->fd, e->file_size);
	if (!e->file_map)
	{
		close (e->fd);
		return (1);
	}
	e->elf_header = get_elf_header_64(e);
	if (!e->elf_header)	
		return (1);
	e->sectionsHeader = get_sections_header_64(e);
	if (!e->sectionsHeader)
		return (1);
	// munmap(map, e->file_size);
	
	e->shstrtab = get_section_by_header_64(e, &e->sectionsHeader[e->elf_header->e_shstrndx]);
	if (!e->shstrtab)
		return (1);
	return (0);
}
int	copy_elf_64(t_elf64 *e, char *filename)
{
	e->file_map = NULL;

	int fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		perror(filename);
		return (1);
	}
	e->filename = filename;

	e->file_size =  lseek(fd, 0, SEEK_END);
	if (e->file_size == 0)//Il faut verifier cette conditions 
	{
		close (fd);
		return (1);
	}
	e->file_map = get_file_in_a_map_64(fd, e->file_size);
	if (!e->file_map)
	{
		close (fd);
		return (1);
	}
	close (fd);
	int woody_fd = open("woody", O_CREAT | O_RDWR | O_TRUNC, 0755);
	if (woody_fd == -1)
		return (1);
	write(woody_fd, e->file_map, e->file_size);
	close(woody_fd);
	munmap(e->file_map ,e->file_size);
	return (0);
}
int insert_pt_load(t_elf64 *e,Elf64_Phdr *Phdr, char *code, int lencode, int i)
{
	uint64_t fin_segment = Phdr[i].p_offset + Phdr[i].p_filesz;
	uint64_t padding = Phdr[i + 1].p_offset - fin_segment;
	uint64_t new_entry_point = Phdr[i].p_vaddr + Phdr[i].p_filesz;
	if (padding < (int64_t)lencode)
		return(1);
	Phdr[i].p_filesz += lencode;	
	Phdr[i].p_memsz  += lencode;
	
	ft_memcpy((char *)e->file_map  + fin_segment, code, lencode);
	ft_memcpy((char *)e->file_map  + 0x18, &new_entry_point, 8);
	return(0);
}
int insert_pt_note(t_elf64 *e,Elf64_Phdr *Phdr, char *code, int lencode, int i, int page_size)//Il fauut mettre une conditions sur la len
{
	Phdr[i].p_type = PT_LOAD;
	Phdr[i].p_flags = PF_R | PF_X; 
	Phdr[i].p_filesz += lencode;	
	Phdr[i].p_memsz  += lencode;
	
	ft_memcpy((char *)e->file_map + Phdr[i].p_offset , code, lencode);
	ft_memcpy((char *)e->file_map + 0x18 , &Phdr[i].p_vaddr, 8);
	return(0);
}

int insert_payload(t_elf64 *e)
{
	char code[] ="\x31\xc0\x99\xb2\x0a\xff\xc0\x89\xc7\x48\x8d\x35\x12\x00\x00\x00\x0f\x05"
    "\xb2\x2a\x31\xc0\xff\xc0\xf6\xe2\x89\xc7\x31\xc0\xb0\x3c\x0f\x05\x2e\x2e"
    "\x57\x4f\x4f\x44\x59\x2e\x2e\x0a";
	int lencode = sizeof(code)  - 1;
	int i;
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	int save ;
	i = 0;
	while(i < e->elf_header->e_phnum - 1)//Elfique transformation n'est pas finit
	{
		printf("i=|%d| phadr aligndb|%lu|", i, Phdr[i].p_align);
		if(Phdr[i].p_type == PT_LOAD && Phdr[i].p_flags &  PF_X)
		{
			if(insert_pt_load(e, Phdr, code, lencode, i) == 0)
				break;
		}
		if(Phdr[i].p_type == PT_NOTE)
		{
			if(insert_pt_note(e, Phdr, code, lencode, i, save) == 0)
				break;
		}
		i++;
	}
	return (0);
}
void print_byte_bits(unsigned char byte)
{
    for (int i = 7; i >= 0; i--)
        printf("%d", (byte >> i) & 1);
}

int print_fd_bits(t_elf64 *e)
{
	
	unsigned char buf[4096];
    ssize_t       n;
    size_t        byte_count = 0;
	
    // Rewind au début du fichier
    int t = lseek(e->fd, 0, SEEK_SET);
	// printf("\nZZZ|fd|%d|lseekk|%d|\n", e->fd, t) ;

    while ((n = read(e->fd, buf, sizeof(buf))) > 0)
    {
        for (ssize_t i = 0; i < n; i++)
        {
            // Offset tous les 4 octets pour lisibilité
            if (byte_count % 4 == 0)
                printf("\n[0x%06zx] ", byte_count);
            else
                printf(" ");

            print_byte_bits(buf[i]);
            byte_count++;
        }
    }
	return(0);
    printf("\n");
}


unsigned char	*get_section_by_header_64_unsigned_char(t_elf64 *e, Elf64_Shdr *sectionHeader)
{
	if (sectionHeader->sh_offset + sectionHeader->sh_size > (unsigned long)e->file_size)
			return (NULL);

	unsigned char *ptr = e->file_map + sectionHeader->sh_offset;
	return (ptr);
}

char	*get_section_by_header_64(t_elf64 *e, Elf64_Shdr *sectionHeader)
{
if (sectionHeader->sh_offset + sectionHeader->sh_size > (unsigned long)e->file_size)
		return (NULL);

	char *ptr = e->file_map + sectionHeader->sh_offset;

	return (ptr);
}
Elf64_Shdr	*get_section_header_by_name_64(t_elf64 *e, const char *name)
{
	int i = 1;

	while (i < e->elf_header->e_shnum)
	{
		if (!ft_strncmp(name, e->shstrtab + e->sectionsHeader[i].sh_name, ft_strlen(name)) 
			&& ft_strlen(name) == ft_strlen(e->shstrtab + e->sectionsHeader[i].sh_name))
			break ;
		i++;
	}
	if (i == e->elf_header->e_shnum)
	{
		return (NULL);
	}
	return (&e->sectionsHeader[i]);
}

// int search_in_case(t_haufman *stack, unsigned char search, int longueur_max)
// {
// 	for(int i = 0; i < longueur_max; i++)
// 	{
// 		if(search == stack[i].type)
// 		{
// 			stack[i].nbr_occurence++;
// 			return(0);
// 		}
// 	}
// 	return(1);
// }

// int search_nbr_two(t_haufman *stack, int max, int save_one, int clef_one)
// {
// 	for (int k = 0; k < max ; k++)
// 	{
// 		if(stack[k].nbr_occurence <= save_one && k != clef_one)
// 		{
			
// 			printf("La clef est 2 em |%d| et la 1||Valeur =|%d|\n", k, stack[k].nbr_occurence);
// 			return(k);
// 		}
// 	}
// 	// if(max == 1);
// 	// 	return(0);
// 	return(search_nbr_two(stack, max, save_one+1, clef_one));
// }
// void print_hold(t_haufman **hold, int max)
// {
//     int total_steps = max;
//     int elements = max;
    
//     for (int s = 0; s < total_steps; s++)
//     {
//         printf("=== STEP %d ===\n", s);
//         for (int i = 0; i < elements; i++)
//         {
//             printf("i=|%d| type=|%02x| occ=|%d|  ",
//                 i,
//                 hold[s][i].type,
//                 hold[s][i].nbr_occurence);
//             if (hold[s][i].left)
//                 printf("L=|%02x| ", hold[s][i].left->type);
//             else
//                 printf("L=|NULL| ");
//             if (hold[s][i].right)
//                 printf("R=|%02x|", hold[s][i].right->type);
//             else
//                 printf("R=|NULL|");
//             printf("\n");
//             printf("SELF=%p L=%p R=%p\n",
//                 (void*)&hold[s][i],
//                 (void*)hold[s][i].left,
//                 (void*)hold[s][i].right);
//         }
//         printf("\n");
//         elements--;
//     }
// }

// void three_haufman(t_haufman **stack, int max , int x_hold)
// {
// 	if(max ==1)
// 		return ;
// 	int save_one = 999999999;
// 	int save_clef =0;
// 	int save_two = 999999999;
// 	int j = 0;
// 	for(j = 0; j < max ; j++)
// 	{
// 		if(save_one >  stack[x_hold][j].nbr_occurence)
// 		{
// 			save_one =  stack[x_hold][j].nbr_occurence;
// 			save_clef = j;
// 		}
// 	}
// 	int search_two = search_nbr_two(stack[x_hold] ,max,  save_one , save_clef);
// 	//Mise en  place de la sauvegarde de right et left
// 	//Save noeud 1
// 	t_haufman *node_one; 
// 	node_one = malloc(sizeof(t_haufman) * 1);
// 	*node_one = stack[x_hold][save_clef];//Je dois peux etre utiliser un ft_memcopy
// 	t_haufman *node_two; 
// 	node_two = malloc(sizeof(t_haufman) * 1);
// 	*node_two = stack[x_hold][search_two];//Je dois peux etre utiliser un 
// 	if(save_clef == max)
// 	{
// 		printf("\nSavetwo\n");
// 		stack[x_hold][search_two].nbr_occurence = stack[x_hold][search_two].nbr_occurence + stack[x_hold][save_clef].nbr_occurence;
// 		stack[x_hold][search_two].right = node_two;
// 		stack[x_hold][search_two].left = node_one;
// 		stack[x_hold][search_two].type = ' ';
// 	}
// 	else
// 	{
// 		stack[x_hold][save_clef].nbr_occurence = node_one->nbr_occurence + node_two->nbr_occurence;
// 		stack[x_hold][save_clef].right = node_two;
// 		stack[x_hold][save_clef].left = node_one;
// 		stack[x_hold][save_clef].type = ' ';		
// 		stack[x_hold][search_two] = stack[x_hold][max - 1];
// 	}
// 	ft_memcpy(stack[x_hold + 1], stack[x_hold],(max )* sizeof(t_haufman)); //Hesite a mettre le -1
// 	printf("\nSaveOne\n");
// 	max--;
// 	if (max != 0 )
// 		three_haufman(stack, max, x_hold + 1);
// 	return;
// }

// void implement_code_table(int code, int depth, t_table_haufman *tabe)
// {
//     tabe->len  = depth;
//     tabe->code = malloc(depth + 1);
//     int j = 0;
//     for (int i = depth - 1; i >= 0; i--)
//     {
//         tabe->code[j] = ((code >> i) & 1) + '0';
//         j++;
//     }
//     tabe->code[j] = '\0';
// }

// void implement_table(t_haufman *noeuds, int step, int code, t_table_haufman *tabe_codage, int *iterateur_table)
// {
//     if (noeuds->left == NULL && noeuds->right == NULL)
//     {
// 		tabe_codage[*iterateur_table].type = noeuds ->type;
//         implement_code_table(code, step, &tabe_codage[*iterateur_table]);
// 		(*iterateur_table)++;
//         return;
//     }
// 	if(noeuds->left != NULL)
// 	{
// 		implement_table(noeuds->left, step + 1, code << 1, tabe_codage, iterateur_table);	
// 	}
// 	if(noeuds->right != NULL)
// 	{
// 		implement_table(noeuds->right, step + 1, (code << 1) | 1, tabe_codage, iterateur_table);
// 	}
// }
// void print_tabe(t_table_haufman *tabe, int size)
// {
//     int i = 0;
//     while (i < size)
//     {
//         printf("i=|%d| type=|%d| len=|%d| code=|%s|\n",
//                i,
//                tabe[i].type,
//                tabe[i].len,
//                tabe[i].code ? tabe[i].code : "NULL");
//         i++;
//     }
// }

// int ft_nbr_type(unsigned char *test, int y , t_haufman **stack, int nbr_max, int limits)//Probablement un free ici 
// {
// 	int nbr = 0;
// 	int nbr_table = 0;
// 	int j;
	
// 	int save = 0;
// 	unsigned char table[limits];
// 	table[0] = '\0';
// 	for(int i = 0; i < limits; i++)
// 	{ 
// 		for(j = 0; j < nbr; j++)
// 			if(test[i] == table[j])
// 				break;
// 		if (j == nbr)
// 		{	
			
// 			table[j] = test[i];
// 			table[j + 1] = '\0';
// 			nbr++;
// 		}
// 	}
// 	if(y == 1)
// 	{
// 			for(int k = 0; k<nbr; k++)
// 			{
// 				stack[0][k].type = table[k];
// 			}
		
// 	}
// 	if(y == 2)
// 	{
// 		for(int g = 0; g < limits;g++)
// 		{
// 			for(int w = 0; w < nbr_max ;w++)
// 			{
// 				if(test[g] == stack[0][w].type)	
// 				{
// 					stack[0][w].nbr_occurence++;					
// 				}
// 			}
// 		}
// 	}
// 	return(nbr);
	
// }

// char *decompresser(t_table_haufman *table_codage, char *crypter, int nbr_type)
// {
// 	char *decrypter = ft_strdup("");
// 	char *save = ft_strdup("");
// 	char **verif = ft_split(crypter, ' ');
// 	char tmp[2];
// 	for (int i = 0; verif[i] != NULL; i++)
// 	{
// 		for (int k = 0; k < nbr_type; k++)
// 		{
// 			if(ft_strncmp(table_codage[k].code, verif[i], ft_strlen(verif[i])) == 0)
// 			{
// 				printf("%02x ", table_codage[k].type );
// 				tmp[0] =  table_codage[k].type;
// 				tmp[1] = '\0';
// 				save = ft_strjoin(decrypter, tmp);
// 				free(decrypter);
// 				decrypter = save;
// 				break;
// 			}
// 		}
// 	}
// 	return (decrypter);
// }

// char *compresser(t_table_haufman *table_codage, unsigned char *test, int nbr_type, int long_max_test)
// {
// 	char *save_crypter  = ft_strdup("");
// 	char *tmp;
// 	char *tmp1;
// 	for (int i = 0; i < long_max_test; i++)
// 	{
// 		for(int k = 0; k < nbr_type ; k++)
// 		{
// 			if(table_codage[k].type  == test[i])
// 			{
// 				tmp1 = ft_strjoin(save_crypter," ");
//                 tmp = ft_strjoin(tmp1, table_codage[k].code);
//                 free(save_crypter);	close(e.fd);

//                 save_crypter = tmp;
//                 break;				
// 			}
// 		}		
// 	}
// 	return(save_crypter);
// }

// int	compression_Decompression(t_elf64 *e)
// {
// 	Elf64_Shdr	*textHeader = get_section_header_by_name_64(e, ".text");
// 	unsigned char	*test = get_section_by_header_64_unsigned_char(e, textHeader);
// 	// //Mise en place du test
// 	t_haufman  **stack;
	
// 	printf("\nTailler=|%lu|", textHeader->sh_size);
// 	int nbr_test = ft_nbr_type(test, 0, stack, 0, textHeader->sh_size);//Calcule le nbr de type
// 	stack = malloc(sizeof(t_haufman*) * (nbr_test + 1));//Vient d'enlver 100
// 	for (int i = 0; i < nbr_test + 1	; i++)
// 	{
//     	stack[i] = malloc(sizeof(t_haufman) * (nbr_test + 1));
//     	if (!stack[i])
//     	    return (1);
//     	ft_memset(stack[i], 0, sizeof(t_haufman) * (nbr_test + 1));
// 	}
// 	ft_nbr_type(test, 1, stack, 0, textHeader->sh_size);//Implement les types	
// 	ft_nbr_type(test, 2, stack, nbr_test, textHeader->sh_size);//Implementes les occurences de types
// 	int test_nbr_occurence = 0;
// 	ft_memcpy(stack[0 + 1], stack[0],(nbr_test + 1)* sizeof(t_haufman)); //Hesite a mettre le -1
// 	three_haufman(stack,nbr_test ,1);
// 	// print_hold(stack , nbr_test);
	
// 	// //Mise en place de la table de decodage
// 	t_table_haufman table_codage[nbr_test];
// 	int iterateur_table_codage = 0;
// 	implement_table(&stack[nbr_test][0], 0, 0, table_codage, &iterateur_table_codage );
// 	// print_tabe(table_codage, iterateur_table_codage);
// 	char *crypter = compresser(table_codage, test, nbr_test, textHeader->sh_size);
// 	// printf("SaveCrypter = %s", crypter);
// 	char  *decrypter=decompresser(table_codage, crypter, nbr_test);
// 	// printf("\n\n\nDecrypte = \n\n|%s|\n", decrypter);
// 	printf("\nOriginal \n");
// 	for(int h = 0; h < textHeader ->sh_size; h++)
// 	{
// 		printf("%02x ", test[h]);
// 	}
// 	return (0);
// }


int	woody_64(char *filename)
{
	t_elf64 e;
	
	if (copy_elf_64(&e, filename))
		return (1);

	if (init_elf_64(&e, "woody"))
		return (1);

	// insert_payload(&e);
	// getSearchSection64(e.elf_header->e_shnum,e.shstrtab,e.sectionsHeader, ".text", e.fd, e.file_size);
	compression_decompression(&e);
	close(e.fd);
	munmap(e.file_map, e.file_size);
	return (0);
}
