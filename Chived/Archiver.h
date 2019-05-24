#pragma once

#include <string>


enum class Algo;

class Archiver
{
public:
	static void createArchive(std::string sourcePath);
	static void createArchive(std::string sourcePath, std::string targetPath);
	static void createArchive(int count, const char** paths, std::string targetPath, std::string archiveName, Algo compression);

	static void extractArchive(std::string archivePath, std::string targetPath);
};
