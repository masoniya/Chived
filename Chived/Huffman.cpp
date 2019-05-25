#include <fstream>
#include <sstream>
#include <bitset>
#include <iostream>

#include "Huffman.h"


#define MAX_TREE_HT 512


void Huffman::compress(std::stringstream & rawData, std::ostream & compressedData)
{
	//count the frequencies from the file and build a map from it
	std::string rawDataString = rawData.str();
	size_t rawSize = rawDataString.length();

	build_Frequencies(rawDataString);

	std::cout << "finished building freqs" << std::endl;

	//build huffman tree and encoding map 
	buildTree();

	std::cout << "finished building tree" << std::endl;


	char bilb; //bits in last byte
	int charcount; //number of chars and frequencies

	charcount = (int)frequencies.size();

	std::vector<char> huffchars; //encoded characters
	std::vector<int> freqs; //character frequencies
	
	//get huffman codes metadata
	for (auto it = frequencies.begin(); it != frequencies.end(); ++it) {
		huffchars.push_back(it->first);
		freqs.push_back(it->second);
	}


	std::stringstream fullbits;
	std::vector<char> fullvec;
	for (int i = 0; i < rawDataString.length(); i++) {
		char nextchar = rawDataString.at(i);

		for (int j = 0; j < encode[nextchar].length(); j++) {
			fullvec.push_back(encode[nextchar][j]);
		}
		//std::string temp(&encode[nextchar][0]);
		//fullbits.write(&encode[nextchar][0], encode[nextchar].length());
	}

	std::cout << "finished building fullstr" << std::endl;

	//std::string fullStr = fullbits.str();
	std::string fullStr = fullvec.data();

	bilb = fullStr.length() % 8;

	std::vector<unsigned char> outputBuffer;

	for (int i = 0; i < fullStr.length(); i += 8) {
		unsigned char byte = 0;
		std::string str8 = "";
		if (i + 8 < fullStr.length()) {

			str8 = fullStr.substr(i, i + 8);
		}
		else {
			str8 = fullStr.substr(i, fullStr.size());
		}

		for (unsigned b = 0; b != 8; ++b) {
			if (b < str8.length()) {
				byte |= (str8[b] & 1) << b;
			}
			else
				byte |= 1 << b;
		}
		outputBuffer.push_back(byte);
	}

	std::cout << "finished filling output buffer" << std::endl;

	//write metadata to file
	compressedData.put(bilb);
	compressedData.write((const char*)&charcount, sizeof(charcount));
	//compressedData.put(charcount);
	compressedData.write(huffchars.data(), charcount);
	compressedData.write((const char*)freqs.data(), charcount * sizeof(int));
	compressedData << "Data\n";

	//write compressed data to file
	compressedData.write((const char*)outputBuffer.data(), outputBuffer.size());
	size_t compressedSize = outputBuffer.size();

	std::cout << "Raw Size : " << rawSize << std::endl;
	std::cout << "Compressed Size : " << compressedSize << std::endl;
	std::cout << "Compression Ratio : " << 1.0 * compressedSize / rawSize << std::endl;
}

void Huffman::decompres(std::stringstream & compressedData, std::iostream & rawData, char bits_in_last_byte, 
	int charcount, char * huffchars, int * freqs)
{
	std::string inputBuffer = compressedData.str();

	//rebuild frequencies
	for (int i = 0; i < charcount; i++) {
		frequencies.insert({ huffchars[i],freqs[i] });
	}

	buildTree();

	//reverse the bits read from the file
	int  k = 0, i = 0;

	std::bitset<8> setByte;
	std::string concat = "";
	std::string bitStr = "";
	std::string reverse = "";

	for (i = 0; i < inputBuffer.size(); i++) {
		setByte = inputBuffer.at(i);
		bitStr = setByte.to_string();
		reverse = "";

		if (i == (inputBuffer.size() - 1)) {
			for (k = 7; k >= 8 - bits_in_last_byte; k--) {
				reverse += bitStr[k];
			}
		}
		else {
			for (k = 7; k >= 0; k--) {
				reverse += bitStr[k];
			}
		}
		concat += reverse;
	}

	//decode the bit stream
	std::vector<char> outputBuffer;
	decode(concat, outputBuffer);

	char * arr = outputBuffer.data();
	rawData.write((char*)arr, outputBuffer.size());
}

void Huffman::decode(std::string code, std::vector<char> &output)
{
	HeapNode* node = this->root;
	char c;
	int i = 0;
	while (i <= code.size()) {
		if (node->isLeaf()) {
			output.push_back(node->c);
			node = this->root;
		}
		else {
			c = code[i];

			if (c == '0') {
				if (node->left) {
					node = node->left;
				}
			}
			else if (c == '1') {
				if (node->right) {
					node = node->right;
				}
			}

			i++;
		}

		if (i == code.size() - 2) {
			std::cout << "";
		}
	}
}

void Huffman::buildEncodeMap(HeapNode* root, char buffer[], int count)
{
	if (root->left) {
		buffer[count] = '0';
		buildEncodeMap(root->left, buffer, count + 1);
	}

	if (root->right) {
		buffer[count] = '1';
		buildEncodeMap(root->right, buffer, count + 1);
	}

	if (root->isLeaf()) {
		std::string s(&buffer[0], &buffer[count]);
		this->encode.insert({ root->c, s });
	}
}

void Huffman::buildTree()
{
	this->heap = new MinHeap(this->frequencies);

	this->root = heap->buildTree();

	char charBuffer[MAX_TREE_HT];
	buildEncodeMap(this->root, charBuffer, 0);
}

void Huffman::build_Frequencies(std::string rawDataString)
{
	for (int i = 0; i < rawDataString.length(); i++) {
		char nextchar = rawDataString.at(i);

		if (this->frequencies.find(nextchar) != this->frequencies.end()) {
			this->frequencies[nextchar]++;
		}
		else {
			this->frequencies.insert({ nextchar, 1 });
		}
	}
}
