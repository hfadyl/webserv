
#include "response.hpp"

std::string locationsRoot(parser_server server, std::string location)
{
	for (size_t i = 0; i < server._server_location.size(); i++)
	{
		if (server._server_location[i]._location_path == location)
			return (server._server_location[i]._location_root);
	}
	return (server._server_root);
}

std::string getExtension(std::string fileName)
{
	const char *extension;
	extension = strrchr(fileName.c_str(), '.');

	if (!extension) // NULL protection
		return ("");
	return std::string(extension);
}

std::string getFileType(std::string fileName)
{
	std::string extension;

	extension = getExtension(fileName);
	if (!extension.empty())
	{
		if (extension == ".html")
			return "text/html";
		else if (extension == ".htm")
			return "text/html";
		else if (extension == ".shtml")
			return "text/html";
		else if (extension == ".css")
			return "text/css";
		else if (extension == ".xml")
			return "text/xml";
		else if (extension == ".csv")
			return "text/csv";
		else if (extension == ".gif")
			return "image/gif";
		else if (extension == ".ico")
			return "image/x-icon";
		else if (extension == ".jpeg")
			return "image/jpeg";
		else if (extension == ".jpg")
			return "image/jpeg";
		else if (extension == ".js")
			return "application/javascript";
		else if (extension == ".json")
			return "application/json";
		else if (extension == ".png")
			return "image/png";
		else if (extension == ".pdf")
			return "application/pdf";
		else if (extension == ".svg")
			return "image/svg+xml";
		else if (extension == ".txt")
			return "text/plain";
		else if (extension == ".atom")
			return "application/atom+xml";
		else if (extension == ".rss")
			return "application/rss+xml";
		else if (extension == ".webp")
			return "image/webp";
		else if (extension == ".3gpp")
			return "video/3gpp";
		else if (extension == ".3gp")
			return "video/3gpp";
		else if (extension == ".ts")
			return "video/mp2t";
		else if (extension == ".mp4")
			return "video/mp4";
		else if (extension == ".mpeg")
			return "video/mpeg";
		else if (extension == ".mpg")
			return "video/mpeg";
		else if (extension == ".mov")
			return "video/quicktime";
		else if (extension == ".webm")
			return "video/webm";
		else if (extension == ".flv")
			return "video/x-flv";
		else if (extension == ".m4v")
			return "video/x-m4v";
		else if (extension == ".mng")
			return "video/x-mng";
		else if (extension == ".asx")
			return "video/x-ms-asf";
		else if (extension == ".asf")
			return "video/x-ms-asf";
		else if (extension == ".wmv")
			return "video/x-ms-wmv";
		else if (extension == ".avi")
			return "video/x-msvideo";
		else if (extension == ".mml")
			return "text/mathml";
		else if (extension == ".txt")
			return "text/plain";
		else if (extension == ".jad")
			return "text/vnd.sun.j2me.app-descriptor";
		else if (extension == ".wml")
			return "text/vnd.wap.wml";
		else if (extension == ".htc")
			return "text/x-component";
		else if (extension == ".png")
			return "image/png";
		else if (extension == ".tif")
			return "image/tiff";
		else if (extension == ".tiff")
			return "image/tiff";
		else if (extension == ".wbmp")
			return "image/vnd.wap.wbmp";
		else if (extension == ".ico")
			return "image/x-icon";
		else if (extension == ".jng")
			return "image/x-jng";
		else if (extension == ".bmp")
			return "image/x-ms-bmp";
		else if (extension == ".svg")
			return "image/svg+xml";
		else if (extension == ".svgz")
			return "image/svg+xml";
		else if (extension == ".mid")
			return "audio/midi";
		else if (extension == ".midi")
			return "audio/midi";
		else if (extension == ".kar")
			return "audio/midi";
		else if (extension == ".mp3")
			return "audio/mpeg";
		else if (extension == ".ogg")
			return "audio/ogg";
		else if (extension == ".m4a")
			return "audio/x-m4a";
		else if (extension == ".ra")
			return "audio/x-realaudio";
		else if (extension == ".pl")
			return "application/x-perl";
		else if (extension == ".py")
			return "application/x-python";
		else if (extension == ".php")
			return "application/x-php";
		else if (extension == ".cpp")
			return "application/x-c++";
		else if (extension == ".c")
			return "application/x-c";
	}
	return "";
}

bool findLocationPathInUri(std::string uri, std::string path)
{
	if (uri.find(path) != std::string::npos)
	{
		if (path.length() > 1 && (uri[path.length()] == '/' || uri[path.length()] == '\0'))
			return true;
		else if (path.length() == 1 && uri[0] == '/')
			return true;
	}
	return false;
}

// returns std pair of string and its length
std::pair<std::string, int> statusMessage(int statusCode)
{
	std::pair<std::string, int> errorMessage;
	if (statusCode == 200)
		errorMessage.first = "200 OK";
	else if (statusCode == 201)
		errorMessage.first = "201 Created";
	else if (statusCode == 204)
		errorMessage.first = "204 No Content";
	else if (statusCode == 301)
		errorMessage.first = "301 Moved Permanently";
	else if (statusCode == 400)
		errorMessage.first = "400 Bad Request";
	else if (statusCode == 403)
		errorMessage.first = "403 Forbidden";
	else if (statusCode == 414)
		errorMessage.first = "414 Request-URI Too Long";
	else if (statusCode == 404)
		errorMessage.first = "404 Not Found";
	else if (statusCode == 405)
		errorMessage.first = "405 Method Not Allowed";
	else if (statusCode == 409)
		errorMessage.first = "409 Conflict";
	else if (statusCode == 413)
		errorMessage.first = "413 Request Entity Too Large";
	else if (statusCode == 500)
		errorMessage.first = "500 Internal Server Error";
	else if (statusCode == 501)
		errorMessage.first = "501 Not Implemented";
	else if (statusCode == 502)
		errorMessage.first = "502 Bad Gateway";
	else if (statusCode == 505)
		errorMessage.first = "505 HTTP Version Not Supported";
	else
		errorMessage.first = "500 Internal Server Error";

	errorMessage.second = errorMessage.first.length();
	return errorMessage;
}

std::string fileSize(std::string path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (file)
	{
		int size = file.tellg();
		file.close();
		return std::to_string(size);
	}
	return "0";
}

std::string formatted_time(void)
{
	time_t current;
	struct tm *timeinfo;
	char buffer[80];

	time(&current);
	timeinfo = localtime(&current);

	strftime(buffer, 80, "%a,%e %b %Y %X %Z", timeinfo);

	return std::string(buffer);
}

std::string convertFileSize(off_t size)
{
	std::string fileSize;
	if (size < 1024)
		fileSize = std::to_string(size) + "B";
	else if (size < 1048576)
		fileSize = std::to_string(size / 1024) + "KB";
	else if (size < 1073741824)
		fileSize = std::to_string(size / 1048576) + "MB";
	else
		fileSize = std::to_string(size / 1073741824) + "GB";
	return fileSize;
}