#pragma once

#include <iostream>

#include "Tree.h"


class AdaHuffman
{
public:
	AdaHuffman();
	

	static void compress(std::string in , std::string &out);

	static void decompress(std::string in, std::string &out);

};
