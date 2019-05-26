#include <unordered_map>
#include <stack>
#include <cstddef>

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

void Compressor::huffmanDecode(std::stringstream & compressedData, std::iostream & rawData, 
	char bits_in_last_byte, int charcount, char * huffchars, int * freqs)
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

void Compressor::shannonFanoEncode(std::iostream & rawData, std::ostream & compressedData)
{
	std::unordered_map<char, int> frequencies;
	std::unordered_map<char, std::vector<int>> codes; // the faster way of doing this is having bytes or byte arrays for codes rather than vectors

	int rawSize = 0;
	//build frequencies
	while (rawData) {
		char nextchar = rawData.get();
		if (rawData.eof()) {
			break;
		}

		rawSize++;

		if (frequencies.find(nextchar) != frequencies.end()) {
			frequencies[nextchar]++;
		}
		else {
			frequencies.insert({ nextchar, 1 });
		}
	}

	int charcount = (int)frequencies.size();

	//sort the characters
	char * huffchars = new char[charcount];
	int i = 0;
	for (auto it = frequencies.begin(); it != frequencies.end(); it++, i++) {
		huffchars[i] = it->first;
		std::vector<int> code;
		codes.emplace(it->first, code);
	}

	//selection sort
	//Iterate over all sub-arrays of data
	for (i = 0; i < charcount - 1; i++) {

		int min = frequencies[huffchars[i]];
		int minIndex = i;

		//Iterate over each element of the sub-array
		for (int j = i + 1; j < charcount; j++) {
			if (frequencies[huffchars[j]] < min) {
				min = frequencies[huffchars[j]];
				minIndex = j;
			}
		}

		//swap first element with max element
		char temp = huffchars[i];
		huffchars[i] = huffchars[minIndex];
		huffchars[minIndex] = temp;
	}

	struct Bounds
	{
		int start;
		int end;
		int total; // so we don't have to calculate this every loop
	};

	//build the codes
	int start = 0;
	int end = charcount;
	int total = 0;
	for (i = 0; i < charcount; i++) {
		total += frequencies[huffchars[i]];
	}
	std::stack<Bounds> indices;
	Bounds full = { start, end, total };

	if (charcount == 1) {
		codes[huffchars[0]].push_back(0);
	}
	else {
		indices.push(full);
	}

	//special case only 1 character

	while (!indices.empty()) {
		Bounds curr = indices.top();
		indices.pop();

		float halfTotal = 1.0f * curr.total / 2;

		int subTotal = 0;

		if (curr.end - curr.start == 1) {
			continue;
		}

		for (i = curr.start; i < curr.end; i++) {

			if (subTotal + frequencies[huffchars[i]] > halfTotal) {

				//both total are equally close
				//add the last element
				if (std::abs(halfTotal - (subTotal + frequencies[huffchars[i]])) - std::abs(halfTotal - subTotal) < 0.000001f) {
					//std::cout << "Close Enough" << std::endl;
					subTotal += frequencies[huffchars[i]];
					int splitIndex = i + 1;
					Bounds bottom = { curr.start, splitIndex, subTotal };
					Bounds top = { splitIndex, curr.end, curr.total - subTotal };

					//push 1 for the bottom
					for (int j = curr.start; j < splitIndex; j++) {
						codes[huffchars[j]].push_back(1);
					}
					//push 0 for the top
					for (int j = splitIndex; j < curr.end; j++) {
						codes[huffchars[j]].push_back(0);
					}

					indices.push(bottom);
					indices.push(top);
					break;
				}

				//add the last element
				else if (std::abs(halfTotal - (subTotal + frequencies[huffchars[i]])) < std::abs(halfTotal - subTotal)) {
					subTotal += frequencies[huffchars[i]];
					int splitIndex = i + 1;
					Bounds bottom = { curr.start, splitIndex, subTotal };
					Bounds top = { splitIndex, curr.end, curr.total - subTotal };

					//push 1 for the bottom
					for (int j = curr.start; j < splitIndex; j++) {
						codes[huffchars[j]].push_back(1);
					}
					//push 0 for the top
					for (int j = splitIndex; j < curr.end; j++) {
						codes[huffchars[j]].push_back(0);
					}

					indices.push(bottom);
					indices.push(top);
					break;
				}
				
				//don't add the last element
				else {
					int splitIndex = i;
					Bounds bottom = { curr.start, splitIndex, subTotal };
					Bounds top = { splitIndex, curr.end, curr.total - subTotal };

					//push 1 for the bottom
					for (int j = curr.start; j < splitIndex; j++) {
						codes[huffchars[j]].push_back(1);
					}
					//push 0 for the top
					for (int j = splitIndex; j < curr.end; j++) {
						codes[huffchars[j]].push_back(0);
					}

					indices.push(bottom);
					indices.push(top);
					break;
				}

			}
			else {
				subTotal += frequencies[huffchars[i]];
			}
		}
	}

	rawData.clear();
	rawData.seekg(0);

	unsigned char writebyte = 0;
	int bits_so_far = 0;
	int bits_in_last_byte;

	int compressedSize = 0;
	
	std::vector<unsigned char> outputBuffer;

	//get all the input bytes
	while (rawData) {
		char nextchar = rawData.get();
		if (rawData.eof()) {
			break;
		}

		//get all the output bits
		for (i = 0; i < codes[nextchar].size(); i++) {
			//put the bit
			writebyte <<= 1;
			writebyte |= codes[nextchar].at(i);
			bits_so_far++;

			//check if byte is full
			if (bits_so_far == 8) {
				outputBuffer.push_back(writebyte);
				writebyte = 0;
				bits_so_far = 0;

				compressedSize++;
			}
		}
	}
	//last byte
	if (bits_so_far > 0) {
		writebyte <<= (8 - bits_so_far);
		outputBuffer.push_back(writebyte);
		compressedSize++;
	}
	bits_in_last_byte = bits_so_far;

	int * freqs = new int[charcount];
	for (i = 0; i < charcount; i++) {
		freqs[i] = frequencies[huffchars[i]];
	}

	//write metadata
	compressedData.put(bits_in_last_byte);
	compressedData.write((const char*)&charcount, sizeof(charcount));
	compressedData.write(huffchars, charcount);
	compressedData.write((const char*)freqs, charcount * sizeof(int));
	compressedData << "Data\n";

	delete huffchars;
	delete freqs;

	//write compressed data
	compressedData.write((const char*)outputBuffer.data(), outputBuffer.size());

	std::cout << "Raw Size : " << rawSize << std::endl;
	std::cout << "Compressed Size : " << compressedSize << std::endl;
	std::cout << "Compression Ratio : " << 1.0 * compressedSize / rawSize << std::endl;
}

void Compressor::shannonFanoDecode(std::stringstream & compressedData, std::iostream & rawData, char bits_in_last_byte, int charcount, char * huffchars, int * freqs)
{
	std::unordered_map<char, int> frequencies;
	std::unordered_map<char, std::vector<int>> codes;

	//rebuild frequencies
	for (int i = 0; i < charcount; i++) {
		frequencies.insert({ huffchars[i],freqs[i] });
	}

	//sort the characters
	int i = 0;
	for (auto it = frequencies.begin(); it != frequencies.end(); it++, i++) {
		//huffchars[i] = it->first;
		std::vector<int> code;
		codes.emplace(it->first, code);
	}

	//selection sort
	//Iterate over all sub-arrays of data
	//for (i = 0; i < charcount - 1; i++) {

	//	int min = frequencies[huffchars[i]];
	//	int minIndex = i;

	//	//Iterate over each element of the sub-array
	//	for (int j = i + 1; j < charcount; j++) {
	//		if (frequencies[huffchars[j]] < min) {
	//			min = frequencies[huffchars[j]];
	//			minIndex = j;
	//		}
	//	}

	//	//swap first element with max element
	//	char temp = huffchars[i];
	//	huffchars[i] = huffchars[minIndex];
	//	huffchars[minIndex] = temp;
	//}

	struct Bounds
	{
		int start;
		int end;
		int total;
	};

	//rebuild the codes
	int start = 0;
	int end = charcount;
	int total = 0;
	for (i = 0; i < charcount; i++) {
		total += frequencies[huffchars[i]];
	}
	std::stack<Bounds> indices;
	Bounds full = { start, end, total };

	if (charcount == 1) {
		codes[huffchars[0]].push_back(0);
	}
	else {
		indices.push(full);
	}

	//create the codes
	while (!indices.empty()) {
		Bounds curr = indices.top();
		indices.pop();

		float halfTotal = 1.0f * curr.total / 2;

		int subTotal = 0;

		if (curr.end - curr.start == 1) {
			continue;
		}

		for (i = curr.start; i < curr.end; i++) {

			if (subTotal + frequencies[huffchars[i]] > halfTotal) {

				//both total are equally close
				//add the last element
				if (std::abs(halfTotal - (subTotal + frequencies[huffchars[i]])) - std::abs(halfTotal - subTotal) < 0.000001f) {
					//std::cout << "Close Enough" << std::endl;
					subTotal += frequencies[huffchars[i]];
					int splitIndex = i + 1;
					Bounds bottom = { curr.start, splitIndex, subTotal };
					Bounds top = { splitIndex, curr.end, curr.total - subTotal };

					//push 1 for the bottom
					for (int j = curr.start; j < splitIndex; j++) {
						codes[huffchars[j]].push_back(1);
					}
					//push 0 for the top
					for (int j = splitIndex; j < curr.end; j++) {
						codes[huffchars[j]].push_back(0);
					}

					indices.push(bottom);
					indices.push(top);
					break;
				}

				//add the last element
				else if (std::abs(halfTotal - (subTotal + frequencies[huffchars[i]])) < std::abs(halfTotal - subTotal)) {
					subTotal += frequencies[huffchars[i]];
					int splitIndex = i + 1;
					Bounds bottom = { curr.start, splitIndex, subTotal };
					Bounds top = { splitIndex, curr.end, curr.total - subTotal };

					//push 1 for the bottom
					for (int j = curr.start; j < splitIndex; j++) {
						codes[huffchars[j]].push_back(1);
					}
					//push 0 for the top
					for (int j = splitIndex; j < curr.end; j++) {
						codes[huffchars[j]].push_back(0);
					}

					indices.push(bottom);
					indices.push(top);
					break;
				}

				//don't add the last element
				else {
					int splitIndex = i;
					Bounds bottom = { curr.start, splitIndex, subTotal };
					Bounds top = { splitIndex, curr.end, curr.total - subTotal };

					//push 1 for the bottom
					for (int j = curr.start; j < splitIndex; j++) {
						codes[huffchars[j]].push_back(1);
					}
					//push 0 for the top
					for (int j = splitIndex; j < curr.end; j++) {
						codes[huffchars[j]].push_back(0);
					}

					indices.push(bottom);
					indices.push(top);
					break;
				}

			}
			else {
				subTotal += frequencies[huffchars[i]];
			}
		}
	}

	delete huffchars;
	delete freqs;


	/************************/
	/**Decoding starts here**/
	/************************/

	//reverse the keys and codes in the map for lookup
	std::map<std::string, char> codechars;
	size_t longestCode = 0;

	for (auto it = codes.begin(); it != codes.end(); it++) {
		std::string code;

		//turn the code into a string
		std::vector<int> codevec = it->second;
		for (int codebit : codevec) {
			code += (codebit == 0 ? '0' : '1');
		}
		
		//length of longest code is useful for reserving memory and checking for error
		if (code.length() > longestCode) {
			longestCode = code.length();
		}

		codechars.insert({ code, it->first });
	}

	//start decoding the stream

	std::string code;
	code.reserve(longestCode + 1);

	size_t numbytes = compressedData.str().length();

	for (int j = 0; j < numbytes - (bits_in_last_byte == 0 ? 0 : 1); j++) {
		unsigned char readbyte = compressedData.get();

		for (i = 0; i < 8; i++) {
			unsigned char readbit = (readbyte << i);
			readbit >>= 7;
			if (readbit == 0) {
				code += '0';
			}
			else if (readbit == 1) {
				code += '1';
			}

			//search for the code in the map
			if (codechars.find(code) != codechars.end()) {
				//if the code is found put the character in the output
				rawData.put(codechars[code]);
				code.clear();
			}
		}

		if (code.length() > longestCode) {
			std::cout << "Error decoding stream..." << std::endl;
			return;
		}
	}
	
	//handle last byte
	if (bits_in_last_byte > 0) {
		unsigned char readbyte = compressedData.get();

		for (int j = 0; j < bits_in_last_byte; j++) {
			unsigned char readbit = (readbyte << j);
			readbit >>= 7;
			if (readbit == 0) {
				code += '0';
			}
			else if (readbit == 1) {
				code += '1';
			}

			if (codechars.find(code) != codechars.end()) {
				rawData.put(codechars[code]);
				code.clear();
			}
		}
	}
}
