#pragma once

#include <map>
#include <vector>
#include <string>

#include "MinHeap.h"


class Huffman
{
public:
	void compress(std::stringstream &rawData, std::ostream &compressedData);
	void decompres(std::stringstream &compressedData, std::iostream &rawData, char bits_in_last_byte, 
		int charcount, char * huffchars, int * freqs);

private:

	std::map<char, int> frequencies;
	std::map<char, std::string> encode;
	HeapNode* root;
	MinHeap* heap;	

	void buildEncodeMap(HeapNode* root, char buffer[], int count);
	void build_Frequencies(std::string rawDataString);
	void buildTree();
	void decode(std::string code, std::vector<char> &output);
};
