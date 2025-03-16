#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <iostream>
#include <stdlib.h>
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>


int FileReadToBuffer(std::vector<uint8_t>& out_buffer, std::string const& filename)
{
	FILE* filePtr = nullptr;

	errno_t errNo = fopen_s(&filePtr, filename.c_str(), "rb");
	if (errNo || filePtr == 0)
	{
		//DebuggerPrintf(Stringf("Could not find file %s\n", filename.c_str()).c_str());
		return 0;
	}

	fseek(filePtr, 0, SEEK_END);
	size_t fileSize = ftell(filePtr);
	if (out_buffer.size() < fileSize)
	{
		out_buffer.resize(fileSize);
	}

	fseek(filePtr, 0, SEEK_SET);
	//fread(out_buffer, sizeof(uint8_t), fileSize, filePtr);

	for (size_t i = 0; i < fileSize; i++)
	{
		out_buffer[i] = static_cast<uint8_t>(fgetc(filePtr));
	}

	fclose(filePtr);

	return static_cast<int>(fileSize);
}

int FileReadToString(std::string& out_string, std::string const& filename)
{
	std::vector<uint8_t> buffer;
	int fileSize = FileReadToBuffer(buffer, filename);
	buffer.push_back(0);

	out_string = reinterpret_cast<char*>(buffer.data());

	return fileSize;
}

int FileWriteBuffer(std::string const& filename, std::vector<uint8_t> const& buffer)
{
	FILE* filePtr = nullptr;

	errno_t errNo = fopen_s(&filePtr, filename.c_str(), "wb");
	if (errNo || filePtr == 0)
	{
		DebuggerPrintf(Stringf("Could not open file for writing %s\n", filename.c_str()).c_str());
		return 0;
	}

	int numBytesWritten = (int)fwrite(buffer.data(), sizeof(uint8_t), buffer.size(), filePtr);

	fclose(filePtr);

	return numBytesWritten;
}

bool CreateFolder(char const* folderPath)
{
	return CreateDirectoryA(folderPath, NULL);
}

void SplitPath(char const* path, char* out_drive, char* out_directory, char* out_filename, char* out_extension)
{
	const size_t DRIVE_ELEM_COUNT = out_drive ? _MAX_DRIVE : 0;
	const size_t DIR_ELEM_COUNT = out_directory ? _MAX_DIR : 0;
	const size_t FNAME_ELEM_COUNT = out_filename ? _MAX_FNAME : 0;
	const size_t EXT_ELEM_COUNT = out_extension ? _MAX_EXT : 0;

	_splitpath_s(path, out_drive, DRIVE_ELEM_COUNT, out_directory, DIR_ELEM_COUNT, out_filename, FNAME_ELEM_COUNT, out_extension, EXT_ELEM_COUNT);
}

char const* MakePath(char const* drive, char const* directory, char const* filename, char const* extension)
{
	constexpr size_t PATH_SIZE = _MAX_DRIVE + _MAX_DIR + _MAX_FNAME + _MAX_EXT;
	char* path = new char[PATH_SIZE];

	_makepath_s(path, PATH_SIZE, drive, directory, filename, extension);

	return path;
}

int ListAllFilesInDirectory(std::string directory, std::vector<std::string>& out_filenames)
{
	int numFiles = 0;

	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA((directory + "\\*").c_str(), &findFileData);
	FindNextFileA(hFind, &findFileData);
	FindNextFileA(hFind, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		ERROR_RECOVERABLE(Stringf("FindFirstFileA failed, error: %d!", GetLastError()));
	}

	do 
	{
		out_filenames.push_back(findFileData.cFileName);
		numFiles++;
	} while (FindNextFileA(hFind, &findFileData));

	FindClose(hFind);
	return numFiles;
}

std::string RunCommand(std::string const& command)
{
	FILE* pipe = _popen(command.c_str(), "r");
	if (!pipe)
	{
		return "Error executing command";
	}

	char buffer[256];
	std::string result = "";
	while (fgets(buffer, sizeof(buffer), pipe))
	{
		result += buffer;
	}
	_pclose(pipe);

	return result;
}

bool IsFileReadOnly(std::string const& filename)
{
	DWORD fileAttributes = GetFileAttributesA(filename.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		ERROR_AND_DIE(Stringf("Error getting file attributes: %d", GetLastError()));
	}
	return (fileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
}
