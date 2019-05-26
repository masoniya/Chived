#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Archiver.h"
#include "Compressor.h"
#include "Engine.h"

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

	/*Engine engine;

	try {
		engine.start();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;*/

	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial" }; // store example
	//const char * path[1] = { "D:\\Temp Files\\testfile.txt" }; // rle example
	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial\\code\\01_instance_creation.htm" }; // adaptive huffman example
	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial\\code" }; // huffman example
	//const char * path[1] = { "D:\\Temp Files\\test.txt" }; // shannon-fano example
	const char * path[1] = { "D:\\Archives\\vulkan-tutorial\\code\\01_instance_creation.htm" }; // shannon-fano example
	//const char * path[1] = { "D:\\Archives\\vulkan-tutorial" };  // shannon-fano example ~10 sec encoding / ~17 sec decoding

	std::string archiveName = "stuff.car";
	//std::string extractDirectory = "C:\\Users\\ASUS\\Desktop";
	std::string extractDirectory = tmp.string();
	Algo compression = Algo::shan_fano;

	Archiver::createArchive(1, path, tmp.string(), archiveName, compression);

	std::string extractName = (tmp / "stuff.car").string();

	//Archiver::extractArchive(extractName, tmp.string());
	Archiver::extractArchive(extractName, extractDirectory);

	getchar();
	return 0;
}
