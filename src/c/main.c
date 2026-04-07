#include "woody64.h"

int main(int argc, char **argv)
{
	t_woody w;

	if (argc != 2)
		return (1);

	if (check_file(argv[1]))
		return (1);
	
	if (init_elf_64(&w, argv[1]))
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
}
