#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


enum class Algo {store, rle, huff, adahuff, shan_fano};

//single function compression/decompression algorithms
class Compressor
{
public:
	static void storeData(std::iostream &rawData, std::ostream &compressedData);
	static void unstoreData(std::iostream &compressedData, std::iostream &rawData);

	static void RLEencode(std::iostream &rawData, std::ostream &compressedData);
	static void RLEdecode(std::iostream &compressedData, std::iostream &rawData);

	static void huffmanEncode(std::stringstream &rawData, std::ostream &compressedData);
	static void huffmanDecode(std::stringstream &compressedData, std::iostream &rawData, char bits_in_last_byte, 
		int charcount, char * huffchars, int * freqs);

	static void adaptiveHuffmanEncode(std::stringstream &rawData, std::ostream &compressedData);
	static void adaptiveHuffmanDecode(std::stringstream &compressedData, std::iostream &rawData, char bits_in_last_byte);

	static void shannonFanoEncode(std::iostream &rawData, std::ostream &compressedData);
	static void shannonFanoDecode(std::stringstream &compressedData, std::iostream &rawData, char bits_in_last_byte,
		int charcount, char * huffchars, int * freqs);
};
