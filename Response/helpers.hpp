#pragma once

#include <vector>
#include <string>
#include <dirent.h> // for stat(file size)


// tmp to facilitate debugging
#define BLUE "\e[1;34m"
#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define DEFAULT "\e[0m"

std::string getExtension(std::string fileName);
std::string getFileType(std::string fileName);
bool findLocationPathInUri(std::string s1, std::string s2);
std::string locationsRoot(parser_server server, std::string location);
bool isAllowedMethod(parser_location location, std::string method);
std::string fileContent(std::string path);
std::pair<std::string, std::string> sendResponse(int error, std::string finalPath);
std::string fileSize(std::string path);
std::pair<std::string, int> statusMessage(int statusCode);
std::string    formatted_time(void);
std::string convertFileSize(off_t size);
