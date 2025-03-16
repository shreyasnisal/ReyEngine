#pragma once

#include <string>
#include <vector>

//! \file FileUtils.hpp

/*! \brief Copies the contents of a file to a character buffer
* 
* \param out_buffer The character buffer that the contents of the file should be copied to
* \param filename The path of the file to be read, relative to the location of the game executable
* \return An integer indicating the number of characters that were successfully copied to the buffer
* 
*/
int FileReadToBuffer(std::vector<uint8_t>& out_buffer, std::string const& filename);

/*! \brief Copies the contents of a file to a string
* 
* \param out_string The string that the contents of the file should be copied to
* \param filename The path of the file to be read, relative to the location of the game exceutable
* \return An integer indicating the number of characters that were successfully copied to the string
*
*/
int FileReadToString(std::string& out_string, std::string const& filename);

/*! \brief Writes the contents of the buffer to a file
* \param The filename that the buffer should be written to.
* \param buffer A vector of unsigned chars containing the buffer that should be written to the file.
* \return An integer representing the number of bytes written
* 
*/
int FileWriteBuffer(std::string const& filename, std::vector<uint8_t> const& buffer);

/*! \brief Creates a folder at the path provided if the folder does not exist
* 
* \param folderPath A char const* representing the path at which the folder should be created.
* \return A boolean indicating whether the folder was created or not.
* 
*/
bool CreateFolder(char const* folderPath);

/*! \brief Splits a path into drive, directory, filename and extension
* 
* \param path 
* \param out_drive Output char* for path drive
* \param out_directory Output char* for path directory
* \param out_filename Output char* for filename
* \param out_extension Output char* for file extension
*/
void SplitPath(char const* path, char* out_drive, char* out_directory, char* out_filename, char* out_extension);

/*! \brief Splits a path into drive, directory, filename and extension
*
* \param path
* \param out_drive Output char* for path drive
* \param out_directory Output char* for path directory
* \param out_filename Output char* for filename
* \param out_extension Output char* for file extension
*/
char const* MakePath(char const* drive, char const* directory, char const* filename, char const* extension);

/*! \brief List all files in a directory
* 
* \param out_filenames A vector to which the file names will be appended
* \param path The directory path for which to list files
* \return An integer representing the number of files found
* 
*/
int ListAllFilesInDirectory(std::string directory, std::vector<std::string>& out_filenames);

std::string RunCommand(std::string const& command);

bool IsFileReadOnly(std::string const& filename);
