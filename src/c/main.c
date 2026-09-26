#include "woody64.h"
#include <stddef.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


// #include <cstdio>

int safe_exit(t_woody *w)
{
	if (w->elf.file_map)
		munmap(w->elf.file_map, w->elf.file_size);
	if (w->payload.payload)
		free(w->payload.payload);
	return (1);
}

void insert_to_table_hunfman_in_elf(t_table_haufman* t_table_haufman, int nbr_table, int fd)//Ne pas zapper 
{
    int i; 
    size_t max;

    max = 0;
    i = 0;

    printf("\n");
    printf("Mise en place d'un marquer pour la tete de lecture ici!");
    // patch_marker(w, 0xISISISISISISISIS, &);
    while(i < nbr_table)
    {
        write(fd, &t_table_haufman[i], sizeof(t_table_haufman[i]));//Je n'utiliser pas lseek 
        i++;
    }
}

// int main(int argc, char **argv)
// {
// 	t_woody w;
// 	t_haufman  **stack;
// 	t_table_haufman *table_codage;
	
// 	int save = 0; //A supprimer juste pour verif
// 	bzero_struct(&w);

// 	if (check_args(&w, argc, argv))
// 		return (1);
	
// 	if (init_elf_64(&w))
// 	{
// 		if (w.elf.file_map)
// 			munmap(w.elf.file_map, w.elf.file_size);
// 		return (1);
// 	}

// 	if(implement_table_haufman(&w, stack, &table_codage))
// 		return(1);
		
// 	if (read_payload(&w))
// 	{
// 		if (w.elf.file_map)
// 			munmap(w.elf.file_map, w.elf.file_size);
// 		return (1);
// 	}

// 	//Il faut mettre la compression avant le xor
// 	w.nbr_boucle_haufman = encryption_text(table_codage, &w, w.textHeader, w.text_test, w.size_t_haufman);
	
// 	//Verification mise en place de la desencryption 
// 	verif_encrytpion(table_codage, &w.elf ,w.textHeader, w.text_test, w.size_t_haufman, w.nbr_boucle_haufman);
	
// 	if (xor_pt_load(&w))
// 		return (safe_exit(&w));
	
// 	if (get_injection_offset(&w))
// 		return (safe_exit(&w));

// 	if (patch_payload(&w))
// 		return (safe_exit(&w));

// 	if (injection(&w))
// 		return (safe_exit(&w));

// 	if (copy_into_woody(&w, table_codage))
// 		return (safe_exit(&w));

// 	print_key(&w);
// 	safe_exit(&w);
// 	return (0);
// }

// Source - https://stackoverflow.com/a/76349929
// Posted by Setheron, modified by community. See post 'Timeline' for change history
// Retrieved 2026-09-05, License - CC BY-SA 4.0



/**
 * This is the program code that will be written to the ELF file.
 * All it does is call the exit system call with the value 42.
 */

// int end_elf(char **argv, t_table_haufman* table_codage, int taille_haufman) {
//     int x = (sizeof(table_codage)) *  taille_haufman;  // Espace réservé pour la table de Huffman

//     // Code assembleur pour "Hello, World!"
//     const uint8_t programCode[] = {
//         0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,  // mov rax, 1 (write)
//         0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,  // mov rdi, 1 (stdout)
//         0x48, 0x8d, 0x35, 0x19, 0x00, 0x00, 0x00,  // lea rsi, [rip + 0x19]
//         0x48, 0xc7, 0xc2, 0x0e, 0x00, 0x00, 0x00,  // mov rdx, 14
//         0x0f, 0x05,                                   // syscall
//         0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00,  // mov rax, 60 (exit)
//         0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00,  // mov rdi, 0
//         0x0f, 0x05,                                   // syscall
//         0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a  // "Hello, World!\n"
//     };

//     // --- ELF Header ---
//     Elf64_Ehdr elfHeader = {
//         .e_ident = { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3, ELFCLASS64, ELFDATA2LSB, EV_CURRENT, ELFOSABI_NONE, 0, 0, 0, 0, 0, 0, 0 },
//         .e_type = ET_EXEC,
//         .e_machine = EM_X86_64,
//         .e_version = EV_CURRENT,
//         .e_phoff = sizeof(Elf64_Ehdr),  // Offset du Program Header
//         .e_shoff = 0,
//         .e_flags = 0,
//         .e_ehsize = sizeof(Elf64_Ehdr),
//         .e_phentsize = sizeof(Elf64_Phdr),
//         .e_phnum = 1,
//         .e_shentsize = 0,
//         .e_shnum = 0,
//         .e_shstrndx = 0
//     };

//     // --- Calcul des alignements ---
//     // 1. Aligner p_offset sur 4096
//     // p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr)
//     // On veut p_offset % 0x1000 == 0
//     int p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
//     int p_offset_remainder = p_offset % 0x1000;
//     int p_offset_padding = (p_offset_remainder == 0) ? 0 : (0x1000 - p_offset_remainder);

//     // 2. p_vaddr doit être congruent à p_offset modulo p_align (0x1000)
//     // p_vaddr = 0x400000 + p_offset + p_offset_padding
//     // Comme 0x400000 est déjà aligné sur 0x1000, p_vaddr % 0x1000 == (p_offset + p_offset_padding) % 0x1000 == 0
//     uint64_t p_vaddr = 0x400000 + p_offset + p_offset_padding;

//     // 3. Calculer l'espace total avant le code (p_offset + p_offset_padding + x)
//     int total_before_code = p_offset + p_offset_padding + x;
//     int code_remainder = total_before_code % 0x1000;
//     int code_padding = (code_remainder == 0) ? 0 : (0x1000 - code_remainder);

//     // 4. Mettre à jour e_entry (adresse du début du code)
//     elfHeader.e_entry = p_vaddr + x + code_padding;

//     // --- Program Header ---
//     Elf64_Phdr programHeader = {
//         .p_type = PT_LOAD,
//         .p_flags = PF_R | PF_X,  // Segment lisible et exécutable
//         .p_offset = p_offset + p_offset_padding,  // Aligné sur 4096
//         .p_vaddr = p_vaddr,  // Aligné sur 4096 et congruent à p_offset modulo p_align
//         .p_paddr = 0,
//         .p_filesz = x + p_offset_padding + code_padding + sizeof(programCode),  // Taille du segment dans le fichier
//         .p_memsz = x + p_offset_padding + code_padding + sizeof(programCode),   // Taille du segment en mémoire
//         .p_align = 0x1000  // Alignement sur 4096
//     };

//     // --- Vérification de l'alignement ---
//     if ((programHeader.p_offset % programHeader.p_align) != (programHeader.p_vaddr % programHeader.p_align)) {
//         fprintf(stderr, "Erreur : p_offset (%d) et p_vaddr (0x%lx) ne sont pas congruents modulo p_align (%d) !\n",
//                 programHeader.p_offset, programHeader.p_vaddr, programHeader.p_align);
//         return -1;
//     }

//     // --- Écriture du fichier ---
//     int fd = open("bob", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
//     if (fd == -1) { perror("open"); return -1; }

//     // Écriture des headers ELF et Program Header
//     if (write(fd, &elfHeader, sizeof(Elf64_Ehdr)) == -1) { perror("write ELF header"); close(fd); return -1; }
//     if (write(fd, &programHeader, sizeof(Elf64_Phdr)) == -1) { perror("write program header"); close(fd); return -1; }

//     // Écriture du padding pour aligner p_offset sur 4096
//     uint8_t *p_offset_pad = calloc(p_offset_padding, 1);
//     if (write(fd, p_offset_pad, p_offset_padding) == -1) {
//         perror("write p_offset padding");
//         free(p_offset_pad);
//         close(fd);
//         return -1;
//     }
//     free(p_offset_pad);

//     // Écriture de la zone x (pour Huffman)
//     int k;
//         print_all_table_haufman(table_codage, taille_haufman);
//         uint8_t *x_bytes = calloc(x, 1);  // Initialisé à 0 (à remplacer par la table de Huffman)
//         if (write(fd, x_bytes, x) == -1) {
//             perror("write x bytes");
//             free(x_bytes);
//             close(fd);
//             return -1;
//     // }
//     //Ne pas xa
//     }
//     free(x_bytes);

//     // Écriture du padding pour aligner le code sur 4096
//     uint8_t *code_pad = calloc(code_padding, 1);
//     if (write(fd, code_pad, code_padding) == -1) {
//         perror("write code padding");
//         free(code_pad);
//         close(fd);
//         return -1;
//     }
//     free(code_pad);

//     //MIse en place header HUfma
//     //Coppie de la taille   
//     if (write(fd, &taille_haufman, sizeof(taille_haufman)) == -1) {//Je copie la zone memoire
//         perror("write program code");
//         close(fd);
//         return -1;
//     }
//     //J'inserre la table dans l'elf 
//     insert_to_table_hunfman_in_elf(table_codage,taille_haufman, fd);

//     //Reecrire la tete de lecture ici
//     //Start_with_table_hufman ici;          

//     // Debug
//     printf("--- Debug Info ---\n");
//     printf("p_offset (avant padding) = %d\n", p_offset);
//     printf("p_offset_padding = %d\n", p_offset_padding);
//     printf("p_offset (final) = %d (0x%x)\n", programHeader.p_offset, programHeader.p_offset);
//     printf("p_vaddr = 0x%lx\n", programHeader.p_vaddr);
//     printf("x = %d\n", x);
//     printf("code_padding = %d\n", code_padding);
//     printf("e_entry = 0x%lx\n", elfHeader.e_entry);
//     printf("p_offset %% p_align = %d\n", programHeader.p_offset % programHeader.p_align);
//     printf("p_vaddr %% p_align = %ld\n", programHeader.p_vaddr % programHeader.p_align);
//     printf("p_filesz = %ld\n", programHeader.p_filesz);
//     printf("p_memsz = %ld\n", programHeader.p_memsz);

//     close(fd);
//     printf("ELF file created successfully: %s\n", argv[1]);
//     return 0;
// }

int main(int argc, char *argv[]) {
  
    if (argc != 2) {
            printf("Usage: %s <output_filename>\n", argv[0]);
            return 1;
    }
	//MIse en place de la table de huffamn 
	t_woody w;
	t_haufman  **stack;
	t_table_haufman *table_codage; 
	bzero_struct(&w);

	//Mise en pinit_elf_64lace de la verification des arguments 
	if(check_args(&w, argc, argv))
		return (1); 
	// //Mise en place de la creation de l'elf minimal 

	if (init_elf_64(&w))
	{
		if (w.elf.file_map)
			munmap(w.elf.file_map, w.elf.file_size);
		return (1);
	}
    
	if(implement_table_haufman(&w, stack, &table_codage, w.elf.fd))
		return(1);
// }

	//if (read_payload(&w))
	//{
	//	if (w.elf.file_map)
	//		munmap(w.elf.file_map, w.elf.file_size);
	//	return (1);
	//}

	//Il faut mettre la compression avant le xor
	// w.nbr_boucle_haufman = encryption_text(table_codage, &w, w.textHeader, w.text_test, w.size_t_haufman);
	return(0);
}
	
	// //Verification mise en place de la desencryption 
	// verif_encrytpion(table_codage, &w.elf ,w.textHeader, w.text_test, w.size_t_haufman, w.nbr_boucle_haufman);
	

	//Ce sont toutes les securite
	// if (xor_pt_load(&w))
	// 	return (safe_exit(&w));
	
	// if (get_injection_offset(&w))
	// 	return (safe_exit(&w));

	// if (patch_payload(&w))
	// 	return (safe_exit(&w));

	// if (injection(&w))
	// 	return (safe_exit(&w));
    
    // //I faut determiner la place necessaire du tableau de huffman pour l'integrer dans l'elf minimal 
	// print_key(&w);
	
	// int end = end_elf(argv, table_codage, w.size_t_haufman);
    // printf("Taille table de hufman %d, le nombre de table, la mise", sizeof(table_codage), w.size_t_haufman );
	// if (end == -1)
	// {
    //     printf("Probleme lors de la creation de l'elf minimal");
	// 	return (-1);
	// }
    
        // if (copy_into_woody(&w, table_codage))
        //     return (safe_exit(&w));
	// safe_exit(&w);