#include "ProvideOutput.h"

#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Not enough input arguments" << std::endl;
		return 1;
	}

	char* addr = argv[1];

	if (ProvideOutput::getInstance(addr).openSock() == 0)
		ProvideOutput::getInstance(addr).waitForInput();
	else
		std::cerr << "main: Couldn't open socket to provide input" << std::endl;
}