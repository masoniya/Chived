#include "Compressor.h"
#include "AdaHuffman.h"
#include "Huffman.h"


void Compressor::storeData(std::iostream &rawData, std::ostream &compressedData)
{
	compressedData << rawData.rdbuf();
}

void Compressor::unstoreData(std::iostream & compressedData, std::iostream & rawData)
{
	rawData << compressedData.rdbuf();
}

void Compressor::RLEencode(std::iostream & rawData, std::ostream & compressedData)
{
	//std::cout << "Started Run Length Encoding" << std::endl;

	size_t rawSize = 0;
	size_t compressedSize = 0;

	unsigned char charcount = 0;
	unsigned char currentchar = (unsigned char)rawData.peek();

	while (rawData) {
		unsigned char nextchar = (unsigned char)rawData.get();
		if (rawData.eof()) {
			compressedData.put(charcount);
			compressedData.put(currentchar);

			compressedSize += 2;

			std::cout << currentchar << " : " << (int)charcount << std::endl;
			break;
		}

		rawSize++;

		if (currentchar == nextchar) {
			charcount++;
		}
		else {
			compressedData.put(charcount);
			compressedData.put(currentchar);

			compressedSize += 2;

			std::cout << currentchar << " : " << (int)charcount << std::endl;
			currentchar = nextchar;
			charcount = 1;
		}

		if (charcount == 255) {
			compressedData.put(charcount);
			compressedData.put(currentchar);

			compressedSize += 2;

			//std::cout << currentchar << " : " << (int)charcount << std::endl;
			currentchar = rawData.peek();
			charcount = 0;
		}
		
	}

	std::cout << "Raw Size : " << rawSize << std::endl;
	std::cout << "Compressed Size : " << compressedSize << std::endl;
	std::cout << "Compression Ratio : " << 1.0 * compressedSize / rawSize << std::endl;

	//std::cout << "Finished Run Length Encoding" << std::endl;
}

void Compressor::RLEdecode(std::iostream & compressedData, std::iostream & rawData)
{
	//std::cout << "Started Run Length Decoding" << std::endl;
	unsigned char charcount;
	unsigned char currentchar;

	while (compressedData) {
		charcount = (unsigned char)compressedData.get();
		currentchar = (unsigned char)compressedData.get();
		if (compressedData.eof()) {
			break;
		}

		for (int i = 0; i < charcount; i++) {
			rawData.put(currentchar);
		}
	}

	//std::cout << "Finished Run Length Decoding" << std::endl;
}

void Compressor::huffmanEncode(std::stringstream & rawData, std::ostream & compressedData)
{
	Huffman algorithm;

	algorithm.compress(rawData, compressedData);
}

void Compressor::huffmanDecode(std::stringstream & compressedData, std::iostream & rawData, char bits_in_last_byte, char charcount, char * huffchars, int * freqs)
{
	Huffman algorithm;

	algorithm.decompres(compressedData, rawData, bits_in_last_byte, charcount, huffchars, freqs);
}

void Compressor::adaptiveHuffmanEncode(std::stringstream & rawData, std::ostream & compressedData)
{
	AdaHuffman algorithm;

	std::string buffer;

	std::string rawString = rawData.str();
	algorithm.compress(rawString, buffer);

	char bits_in_last_byte = buffer.length() % 8;

	compressedData << bits_in_last_byte << '\n';
	compressedData << "Data\n";

	for (int i = 0; i < buffer.length() - bits_in_last_byte; i += 8) {
		unsigned char writebyte = 0;
		for (int j = 0; j < 8; j++) {
			unsigned char readbit = buffer.at(i + j);
			if (readbit == '0') {
				readbit = 0;
			}
			else if (readbit == '1') {
				readbit = 1;
			}
			writebyte <<= 1;
			writebyte |= readbit;
		}
		compressedData.put(writebyte);
	}

	if (bits_in_last_byte != 0) {
		size_t offset = buffer.length() - bits_in_last_byte;
		unsigned char writebyte = 0;
		for (int j = 0; j < bits_in_last_byte; j++) {
			unsigned char readbit = buffer.at(offset + j);
			if (readbit == '0') {
				readbit = 0;
			}
			else if (readbit == '1') {
				readbit = 1;
			}
			writebyte <<= 1;
			writebyte |= readbit;
		}
		writebyte <<= (8 - bits_in_last_byte);
		compressedData.put(writebyte);
	}

	size_t rawSize = rawString.length();
	size_t compressedSize = buffer.length() / 8 + (bits_in_last_byte == 0 ? 0 : 1);

	std::cout << "Raw Size : " << rawSize << std::endl;
	std::cout << "Compressed Size : " << compressedSize << std::endl;
	std::cout << "Compression Ratio : " << 1.0 * compressedSize / rawSize << std::endl;
}

void Compressor::adaptiveHuffmanDecode(std::stringstream & compressedData, std::iostream & rawData, char bits_in_last_byte)
{
	AdaHuffman algorithm;

	std::stringstream bufferstream;

	std::string compressedString = compressedData.str();

	for (int i = 0; i < compressedString.length() - (bits_in_last_byte == 0 ? 0 : 1); i++) {
		unsigned char readbyte = compressedString.at(i);
		for (int j = 0; j < 8; j++) {
			unsigned char writebit = readbyte << j;
			writebit = writebit >> 7;
			if (writebit == 0) {
				writebit = '0';
			}
			else if (writebit == 1) {
				writebit = '1';
			}
			bufferstream.put(writebit);
		}
	}

	if (bits_in_last_byte != 0) {
		unsigned char readbyte = compressedString.at(compressedString.length() - 1);
		for (int j = 0; j < bits_in_last_byte; j++) {
			unsigned char writebit = readbyte << j;
			writebit = writebit >> 7;
			if (writebit == 0) {
				writebit = '0';
			}
			else if (writebit == 1) {
				writebit = '1';
			}
			bufferstream.put(writebit);
		}
	}

	std::string buffer = bufferstream.str();
	std::string rawString;
	algorithm.decompress(buffer, rawString);

	rawData.write(&rawString[0], rawString.length());
}
