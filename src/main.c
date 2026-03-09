#include "woody64.h"

int main(int argc, char **argv)
{
	if (argc != 2)
		return (1);

	int elf_class = find_class(argv[1]);
	if (elf_class == ELFCLASS64)
	{
		woody_64(argv[1]);
	}
}