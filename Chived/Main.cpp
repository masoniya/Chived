#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Archiver.h"
#include "Compressor.h"


namespace fs = std::filesystem;

int main() {

	/**********************************************************/
	/***creating temp directory and cleaning up old archives***/
	/**********************************************************/

	//get temp directory and create a folder there if there isn't
	char* tmpbuffer;
	size_t tmpcount;
	_dupenv_s(&tmpbuffer, &tmpcount, "temp");
	fs::path tmp(tmpbuffer);
	free(tmpbuffer);

	if (!fs::exists(tmp /= "chived")) {
		std::cout << "temp directory does not exist... creating temp directory..." << tmp.string() << std::endl;
		fs::create_directory(tmp);
	}
	else {
		fs::remove_all(fs::path(tmp));
		std::cout << "temp directory already exists... clearing temp directory..." << std::endl;
		fs::create_directory(tmp);
	}

	fs::path archive(tmp / "archive.car");

	std::cout << "The archive : " << archive.string() << std::endl;

	//delete existing temp archive
	if (fs::exists(archive)) {
		std::cout << "Archive already exists... deleting" << std::endl;
		fs::remove(archive);
	}

	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial" }; // store example
	//const char * path[1] = { "D:\\Temp Files\\testfile.txt" }; // rle example
	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial\\code\\01_instance_creation.htm" }; // adaptive huffman example
	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial\\code" }; // huffman example
	const char * path[1] = { "D:\\Archives\\vulkan-tutorial\\code" }; // shannon-fano example


	std::string archiveName = "stuff.car";
	Algo compression = Algo::shan_fano;

	Archiver::createArchive(1, path, tmp.string(), archiveName, compression);

	std::string extractName = (tmp / "stuff.car").string();

	Archiver::extractArchive(extractName, tmp.string());

	getchar();
	return 0;
}
