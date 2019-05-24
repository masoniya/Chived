#pragma once

#include <filesystem>
#include <istream>
#include <sstream>

#include "Archiver.h"
#include "Compressor.h"


namespace fs = std::filesystem;

void Archiver::createArchive(std::string sourcePath)
{
	createArchive(sourcePath, sourcePath);
}

void Archiver::createArchive(std::string sourcePath, std::string targetPath)
{
	const char * paths[1] = { sourcePath.c_str() };
	//createArchive(1, paths, targetPath);
}

void Archiver::createArchive(int count, const char ** paths, std::string targetPath, std::string archiveName, Algo compression)
{
	fs::path * basePaths = new fs::path[count];
	std::ofstream archive;
	std::stringstream tempstream;

	if (fs::is_directory(fs::path(targetPath))) {
		archive = std::ofstream(targetPath + "\\" + archiveName, std::ios_base::binary);
	}
	else {
		archive = std::ofstream(fs::path(targetPath).parent_path().string() + "\\" + archiveName, std::ios_base::binary);
	}

	//create paths
	for (int i = 0; i < count; i++) {
		basePaths[i] = fs::path(paths[i]);
	}

	std::cout << "Creating Archive :" << std::endl;

	//add folder names to archive
	std::cout << "Adding Folders :" << std::endl;
	archive << "Folders\n";
	for (int i = 0; i < count; i++) {
		if (fs::is_directory(basePaths[i])) {
			fs::path baseRelative = fs::relative(basePaths[i], basePaths[i].parent_path());
			archive << baseRelative.string() << '\n'; //add the directory itself

			std::cout << "Added Directory : " << baseRelative.string() << std::endl;

			auto it = fs::recursive_directory_iterator(basePaths[i]);
			for (fs::path subp : it) {
				if (fs::is_directory(subp)) {
					fs::path relative = fs::relative(subp, basePaths[i].parent_path());
					archive << relative.string() << '\n'; //add subdirectories

					std::cout << "Added Directory : " << relative.string() << std::endl;
				}
			}
		}
	}
	archive << '\n';

	//add file names to archive and file data to temp stream
	std::cout << "Adding Files :" << std::endl;
	archive << "Files\n";
	for (int i = 0; i < count; i++) {
		if (fs::is_directory(basePaths[i])) {
			auto it = fs::recursive_directory_iterator(basePaths[i]);
			//add the files in the directory and its subdirectories
			for (fs::path subp : it) {
				if (fs::is_regular_file(subp)) {
					fs::path relative = fs::relative(subp, basePaths[i].parent_path());
					archive << relative.string() << '\n'; //add files in all directories and subdirectories
					archive << fs::file_size(subp) << '\n'; //add file size

					//add file data
					std::ifstream filedata(subp.string(), std::ios_base::binary);
					tempstream << filedata.rdbuf();
					
					std::cout << "Wrote : " << fs::file_size(subp) << " bytes" << std::endl;

					filedata.close();
				}
			}
		}
		else if (fs::is_regular_file(basePaths[i])) {
			fs::path baseRelative = fs::relative(basePaths[i], basePaths[i].parent_path());
			archive << baseRelative.string() << '\n'; //add the file itself
			archive << fs::file_size(basePaths[i]) << '\n';

			std::ifstream filedata(basePaths[i].string(), std::ios_base::binary);
			tempstream << filedata.rdbuf();

			std::cout << "Added File : " << baseRelative.string() << " : " << fs::file_size(basePaths[i]) << " bytes" << std::endl;

			filedata.close();
		}
	}
	archive << '\n';

	//add data to the compression algorithm then add full data stream to the archive
	std::cout << "Compressing Data :" << std::endl;
	archive << "Compression Info\n";
	switch (compression) {
	case Algo::rle:
		archive << "rle\n";
		archive << "Data\n";
		Compressor::RLEencode(tempstream, archive);
		break;
	case Algo::huff:
		archive << "huff\n";
		Compressor::huffmanEncode(tempstream, archive);
		break;
	case Algo::adahuff:
		archive << "adahuff\n";
		Compressor::adaptiveHuffmanEncode(tempstream, archive);
		break;
	case Algo::shan_fano:

	default:
	case Algo::store:
		archive << "store\n";
		archive << "Data\n";
		Compressor::storeData(tempstream, archive);
		break;
	}
	
	std::cout << "Succesfully Created Archive..." << std::endl;

	archive.close();
	tempstream.clear();
}

void Archiver::extractArchive(std::string archivePath, std::string targetPath)
{
	std::ifstream input(archivePath, std::ios_base::binary);

	fs::path foldername = fs::path(archivePath).stem();
	fs::create_directory(fs::path(targetPath) / foldername);

	std::cout << "Extracting Archive :" << std::endl;
	std::string line;
	size_t archivesize = fs::file_size(archivePath);

	std::cout << "Extracing Folders :" << std::endl;
	std::getline(input, line);

	if (line != "Folders") {
		std::cout << "Error extracting archive... \nDirectory Structure Not Found..." << std::endl;
		return;
	}
	
	//Create folders
	while (std::getline(input, line)) {
		if (line == "")
			break;
		fs::create_directories(fs::path(targetPath) / foldername / line);
	}

	std::vector<std::string> filenames;
	std::vector<size_t> filesizes;

	std::cout << "Extracting Files :" << std::endl;
	std::getline(input, line);

	if (line != "Files") {
		std::cout << "Error extracting archive... \nFilenames Not Found..." << std::endl;
		return;
	}

	//Store file names and sizes
	while (std::getline(input, line)) {
		if (line == "")
			break;

		size_t filesize;
		input >> filesize;

		filenames.push_back((fs::path(targetPath) / foldername / line).string());
		filesizes.push_back(filesize);

		std::getline(input, line);
	}

	std::cout << "Extracting File Data :" << std::endl;
	std::getline(input, line);

	if (line != "Compression Info") {
		std::cout << "Error extracting archive... \nCompression Info Not Found..." << std::endl;
		return;
	}

	Algo compression;
	char bilb = 0;
	char charcount = 0;
	char * huffchars = nullptr;
	int * freqs = nullptr;


	std::getline(input, line);
	if (line == "store") {
		compression = Algo::store;
	}
	else if (line == "rle") {
		compression = Algo::rle;
	}
	else if (line == "adahuff") {
		compression = Algo::adahuff;
		input >> bilb;
		std::getline(input, line);
	}
	else if (line == "huff") {
		compression = Algo::huff;

		input.read(&bilb, 1);
		input.read(&charcount, 1);

		huffchars = new char[charcount];
		freqs = new int[charcount];

		input.read(huffchars, charcount);
		input.read((char *)freqs, charcount * sizeof(int));
	}
	else {
		std::cout << "Error extracting archive... \nUnknown Compression Algorithm..." << std::endl;
		return;
	}

	std::getline(input, line);
	if (line != "Data") {
		std::cout << "Error extracting archive... \nData Section Not Found..." << std::endl;
		return;
	}

	//get remaining data
	char * tempbuffer = new char[archivesize];
	input.read(tempbuffer, archivesize);
	std::streamsize dataSectionSize = input.gcount();

	std::stringstream tempstream; //contains compressed data
	tempstream.write(tempbuffer, dataSectionSize);
	delete tempbuffer;

	std::stringstream finalDataStream; //contains uncompressed data

	std::cout << "Decompressing Data :" << std::endl;

	switch (compression) {
	case Algo::rle:
		Compressor::RLEdecode(tempstream, finalDataStream);
		break;
	case Algo::huff:
		Compressor::huffmanDecode(tempstream, finalDataStream, bilb, charcount, huffchars, freqs);
		break;
	case Algo::adahuff:
		Compressor::adaptiveHuffmanDecode(tempstream, finalDataStream, bilb);
		break;
	case Algo::shan_fano:

	default:
	case Algo::store:
		Compressor::unstoreData(tempstream, finalDataStream);
		break;
	}

	for (int i = 0; i < filenames.size(); i++) {
		std::string filename = filenames.at(i);
		size_t filesize = filesizes.at(i);

		char* buffer = new char[filesize];
		finalDataStream.read(buffer, filesize);

		std::ofstream file(filename, std::ios_base::binary);
		file.write(buffer, filesize);

		file.close();
		delete buffer;
		
		std::cout << "Extracted : " << filesize << " bytes to " << fs::path(filename).filename() << std::endl;
	}
	
	input.close();
	tempstream.clear();
	finalDataStream.clear();
}
