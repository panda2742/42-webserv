#include <iostream>
#include "parsing/Parser.hpp"

int	main(int argc, char **argv)
{
	if (argc ^ 2)
	{
		std::cout << "Provide a unique configuration file." << std::endl;
		return 1;
	}
	Parsing::Parser	parser(argv[1]);

	parser.parse();
}