#include "woody64.h"
#include "fcntl.h"
#include <sys/stat.h>
#include "stdio.h"
#include <sys/mman.h>

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
	printf("entry; %ld\n", new_entry_point);
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

int insert_payload_tmp(t_elf64 *e)
{
	char code[] ="\x31\xc0\x99\xb2\x0a\xff\xc0\x89\xc7\x48\x8d\x35\x12\x00\x00\x00\x0f\x05"
    "\xb2\x2a\x31\xc0\xff\xc0\xf6\xe2\x89\xc7\x31\xc0\xb0\x3c\x0f\x05\x2e\x2e"
    "\x57\x4f\x4f\x44\x59\x2e\x2e\x0a";
	int lencode = sizeof(code)  - 1;
	int i;
	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	int save ; // WTF ?
	i = 0;
	while(i < e->elf_header->e_phnum - 1)
	{
		printf("i=|%d| phadr aligndb|%lu|\n", i, Phdr[i].p_align);
		if(Phdr[i].p_type == PT_LOAD && Phdr[i].p_flags &  PF_X)
		{
			if(insert_pt_load(e, Phdr, code, lencode, i) == 0)
				break;
		}
		if(Phdr[i].p_type == PT_NOTE)
		{
			if(insert_pt_note(e, Phdr, code, lencode, i, save)==0)
				break;
		}
		i++;
	}
	close(e->fd);//si break attention 
	return (0);
}

int	segment_is_PT_LOAD(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_LOAD && segment->p_flags & PF_X);
}

int	segment_is_PT_NOTE(Elf64_Phdr *segment)
{
	return (segment->p_type == PT_NOTE);
}

int	enough_zero_padding_in_segment(Elf64_Phdr *segment, int size)
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;

	if (padding > (int64_t)size)
		return (1);
	return (0);
}

uint64_t	insert_in_PT_LOAD(t_elf64 *e, Elf64_Phdr *segment, char *code, int size)
{
	uint64_t fin_segment = segment->p_offset + segment->p_filesz;
	uint64_t padding = segment[1].p_offset - fin_segment;
	uint64_t injection_adress = segment->p_vaddr + segment->p_filesz;

	segment->p_filesz += size;
	segment->p_memsz  += size;
	
	ft_memcpy((char *)e->file_map + fin_segment, code, size);

	return (injection_adress);
}

int insert_payload(t_elf64 *e)
{
	char payload[] = "\x31\xc0\x99\xb2\x0a\xff\xc0\x89\xc7\x48\x8d\x35\x12\x00\x00\x00\x0f\x05"
    "\xb2\x2a\x31\xc0\xff\xc0\xf6\xe2\x89\xc7\x31\xc0\xb0\x3c\x0f\x05\x2e\x2e"
    "\x57\x4f\x4f\x44\x59\x2e\x2e\x0a";

	Elf64_Phdr *Phdr = (Elf64_Phdr *)(e->file_map + e->elf_header->e_phoff);
	uint64_t new_entry_point;
	int i;
	int found = 0; //to do

	for (i = 0; i < e->elf_header->e_phnum - 1; i++)
	{
		if (segment_is_PT_LOAD(&Phdr[i]) && enough_zero_padding_in_segment(&Phdr[i], sizeof(payload) - 1))
		{
			new_entry_point = insert_in_PT_LOAD(e, &Phdr[i], payload, sizeof(payload) - 1);
			ft_memcpy((char *)e->file_map  + 0x18, &new_entry_point, 8);
			break ;
		}
	}
	printf("jumped at %d\n", i);
	if (i == e->elf_header->e_phnum - 1)
	{
		for (i = 0; i < e->elf_header->e_phnum - 1; i++)
		{
			if (segment_is_PT_NOTE(&Phdr[i]))
			{
				//to do
			}
		}
	}
}

int injection(t_elf64 *e)
{
	if (insert_payload(e))
		return (1);

	// if (insert_payload_tmp(e))
	// 	return (1);
}
