#include "woody64.h"

int main(int argc, char **argv)
{
	t_woody w;

	bzero_ptr(&w);

	if (check_args(&w, argc, argv))
		return (1);
	
	if (init_elf_64(&w))
		return (1);

	if (read_payload(&w))
		return (1);

	if (xor_pt_load(&w))
		return (1);
	
	if (get_injection_offset(&w))
		return (1);

	if (patch_payload(&w))
		return (1);

	if (injection(&w))
		return (1);

	if (copy_into_woody(&w))	
		return (1);

	print_key(&w);
	return (0);
}
