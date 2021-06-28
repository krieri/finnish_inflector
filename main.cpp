#include <iostream>

#include "header.h"


int main(int argc, char* argv[])
{
	// test
	std::string stem("soitti");
	std::string mask("n");
	std::string face("me");
	std::string nomSG = merge(stem, mask);
	std::string nomPL = merge(merge(stem, face), "lta");

	printf("%s, %s\n", nomSG.c_str(), nomPL.c_str());

	return 0;
}