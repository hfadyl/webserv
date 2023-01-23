#include "response.hpp"

Response::Response(parser_server server, Request request, int socket_fd)
	: _socket_fd(socket_fd), _server(server), _request(request), _finalPath(""), _cgi(""), _statusCode(""), _contentType(""), _cgiBody("")
{
	this->_response_sent = false;
	this->_uri = _request.get_path();
	this->_method = _request.get_method();

	if (_server._server_error_page.size() == 2)
		this->_error_page = this->getErrorPage();

	if (_request.get_status() != 200 && _request.get_status() != 201)
		sendResponse(_request.get_status(), "");
	else
		startResponse(server);
}

void Response::startResponse(parser_server server)
{
	//  _matchinglocation and _location
	handlePath(server);

	//  check if the method exists in the matching location
	if (!isAllowedMethod(_method))
	{
		// if get or post or delte send 405 otherwise 501
		if (_method == "GET" || _method == "POST" || _method == "DELETE")
			sendResponse(405, "");
		else
			sendResponse(501, "");
	}

	//  check redirection
	else if (_location._location_return.size() > 0)
		sendResponse(301, _location._location_return[1]);

	//  check if directory or file or not found and handle it depending on that
	else
		pathType();
}

Response &Response::operator=(Response const &rhs)
{
	if (this != &rhs)
	{
		this->_socket_fd = rhs._socket_fd;
		this->_server = rhs._server;
		this->_request = rhs._request;
		this->_uri = rhs._uri;
		this->_method = rhs._method;
		this->_location = rhs._location;
		this->_matchingLocation = rhs._matchingLocation;
		this->_finalPath = rhs._finalPath;
		this->_sent = rhs._sent;
	}
	return *this;
}

Response::Response(Response const &src) { *this = src; }

Response::~Response()
{
	_cgiHeaders.clear();
}

/*
 **************************************************************************************************
 */

void Response::pathType()
{
	// if not found
	if (access(this->_finalPath.c_str(), F_OK) != 0)
		sendResponse(404, "");

	// permission denied
	else if (access(this->_finalPath.c_str(), R_OK) != 0)
		sendResponse(403, "");

	// if directory
	else if (DIR *dir = opendir(this->_finalPath.c_str()))
	{
		// std::cout << "sir fhalk" << std::endl;
		handleDirectory(dir);
		closedir(dir);
	}

	// if file
	else
	{
		std::ifstream file(this->_finalPath);
		if (file)
		{
			handleFile();
			file.close();
		}
	}
}

/***************************************************************************************************/

void Response::handleDirectory(DIR *dir)
{
	if (_method == "DELETE")
		DELETE("directory", dir);
	else
		GET(dir);
}

/***************************************************************************************************/

void Response::autoIndex()
{
	// create an index.html file and generate it and fill it by reading directory content
	std::ofstream index("autoindex.html", std::ios::out | std::ios::trunc);
	if (!index)
		throw std::runtime_error("failed to create index.html for autoindex");
	// std::string _currentDir = getcwd(NULL, 0);
	if (index)
	{
		index << "<!DOCTYPE html>"
			  << "<html>"
			  << "<head>"
			  << "<title>Index</title>"
			  << "</head>";

		// index << "<!DOCTYPE html>" << "<html>" << "<head>" << "<title>Index</title>"
		// 	  << "<link rel=\"stylesheet\" href=\"" + _currentDir + "/style.css\">" << "</head>";

		index << "<body>"
			  << "<h1>Index</h1>"
			  << "<table>" << std::endl;

		DIR *dir = opendir(_directory.c_str());
		if (dir)
		{
			autoIndexDirectory(dir, index);
			closedir(dir);
		}
		index << "</table>"
			  << "</body>"
			  << "</html>";
		index.close();
	}
	sendResponse(200, "./autoindex.html");
}

/***************************************************************************************************/

void Response::autoIndexDirectory(DIR *dir, std::ofstream &index)
{
	struct dirent *entry;
	std::string location = _location._location_path;

	if (_uri.empty())
		_uri = "/";
	else if (_uri.size() > 1 && _uri[_uri.size() - 1] != '/')
		_uri += "/";
	if (_uri.size() == 1 && location.size() == 1 && _uri[0] == '/' && location[0] == '/')
		_uri = "";
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0)
		{
			if (entry->d_type == DT_DIR)
				index << "<tr><td><a href=\"" << location << _uri << entry->d_name << "/\">" << entry->d_name << "</a></td><td>directory</td><td></td></tr>";
			else if (entry->d_type == DT_REG)
			{
				struct stat st;
				stat(entry->d_name, &st);
				index << "<tr><td><a href=\"" << location << _uri << entry->d_name << "\">" << entry->d_name << "</a></td><td>file</td><td>";
				index << formatted_time() << "</td><td>" << convertFileSize(st.st_size) << "</td></tr>";
			}
		}
	}
}

/***************************************************************************************************/

bool Response::handleFile()
{
	std::string fileExtension = getExtension(_finalPath);

	if (_method == "DELETE")
		DELETE("file", NULL);
	else
	{
		int checkFile_ = checkFile(this->_finalPath);

		// if successs, or failure with autoindex off then just return
		if (checkFile_ == 200 || (!_location._location_auto_index))
		{
			if (_location._location_cgi != "" && ((fileExtension == ".py" && (_location._location_cgi == "python")) || (fileExtension == ".php" && (_location._location_cgi == "php"))))
			{
				if (fileExtension == ".py")
				{
					_cgi = "/usr/bin/python";
					CGI("python");
				}
				else
				{
					_cgi = "cgi-bin/php-cgi";
					CGI("php");
				}
			}
			else
				sendResponse(checkFile_, this->_finalPath);
			return true;
		}
		else
			return false;
	}
	return true;
}

/***************************************************************************************************/

int Response::checkFile(std::string file)
{
	if (access(file.c_str(), F_OK) != 0)
		return 404;
	else if (access(file.c_str(), R_OK) != 0)
		return 403;
	else
		return 200;
}

/***************************************************************************************************/

void Response::GET(DIR *dir)
{
	// check if folder empty
	if (isEmptyFolder(dir))
	{
		sendResponse(404, "");
		return;
	}

	// if folder has default file
	if (_location._location_default != "")
	{
		_finalPath = (_finalPath[_finalPath.size() - 1] == '/') ? _finalPath : _finalPath + "/";
		_directory = _finalPath; // autoindex will use this
		_finalPath += _location._location_default;

		// std::cout << GREEN << "GET: " << _finalPath << DEFAULT << std::endl;

		// if default file is an existing file
		std::ifstream file(this->_finalPath);
		if (file)
		{
			// handle file return true if file not found or already sent
			if (!handleFile())
				autoIndex();
			file.close();
			return;
		}
	}

	// default file not found or not opened ...
	_directory = _finalPath.substr(0, _finalPath.find_last_of("/"));
	if (_location._location_auto_index)
		autoIndex();
	else
		sendResponse(404, "");
}

/***************************************************************************************************/

void Response::DELETE(std::string pathType_, DIR *dir)
{
	if (pathType_ == "directory")
	{
		if (isEmptyFolderDel(dir))
		{
			rmdir(this->_finalPath.c_str());
			sendResponse(204, "");
		}
		else
			sendResponse(403, "");
	}
	else if (pathType_ == "file")
	{
		if (checkFileAccess(this->_finalPath))
		{
			remove(this->_finalPath.c_str());
			sendResponse(204, "");
		}
		else if (!checkFileAccess(this->_finalPath))
			sendResponse(403, "");
		else
			sendResponse(404, "");
	}
}

/***************************************************************************************************/

std::string Response::longestPath(std::vector<std::string> strings, parser_server server)
{
	size_t max = 0;
	std::string max_match;
	for (size_t i = 0; i < strings.size(); i++)
		if (strings[i].length() > max)
		{
			max = strings[i].length();
			max_match = strings[i];
		}

	// find the matching location depends on location path
	for (size_t i = 0; i < server._server_location.size(); ++i)
		if (max_match == server._server_location[i]._location_path)
			this->_location = server._server_location[i];

	return max_match;
}

/***************************************************************************************************/

std::string Response::matchingLocation(parser_server server)
{
	std::vector<std::string> matches;

	for (size_t i = 0; i < server._server_location.size(); i++)
		if ((findLocationPathInUri(this->_uri, server._server_location[i]._location_path)))
			matches.push_back(server._server_location[i]._location_path);

	if (matches.empty())
		return ("no matching location");

	return longestPath(matches, server);
}

/***************************************************************************************************/

void Response::handlePath(parser_server server)
{
	_matchingLocation = matchingLocation(server);

	if (_matchingLocation == "no matching location")
		this->_finalPath = server._server_root + this->_uri.erase(0, 1);
	else
	{
		std::string root = locationsRoot(server, _matchingLocation);
		if (_matchingLocation.length() == 1)
			this->_finalPath = root + this->_uri.erase(0, 1);
		else
		{
			root = root.substr(0, root.length() - 1);
			this->_finalPath = root + this->_uri.erase(0, _matchingLocation.length());
		}
	}
}

/***************************************************************************************************/

// if the given method is not supported, return false
bool Response::isAllowedMethod(std::string method)
{
	for (size_t i = 0; i < _location._location_method.size(); ++i)
		if (method == _location._location_method[i])
			return true;
	return false;
}

/***************************************************************************************************/

bool Response::isEmptyFolder(DIR *dir)
{
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
		if (strcmp(ent->d_name, "."))
			return false;
	return true;
}

bool Response::isEmptyFolderDel(DIR *dir)
{
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
		if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			return false;
	return true;
}

/***************************************************************************************************/

bool Response::checkFileAccess(std::string file)
{
	if (access(file.c_str(), F_OK) == 0 && access(file.c_str(), R_OK) == 0 && access(file.c_str(), W_OK) == 0)
		return true;
	return false;
}

/***************************************************************************************************/

std::pair<std::string, std::string> Response::getResponse()
{
	return _response;
}

/***************************************************************************************************/

void Response::checkDefaultErrorPage(std::string statusCode)
{
	if (_error_page.size() != 2 || _error_page[0] == "" || _error_page[1] == "")
		_defaultErrorPage = "";

	_defaultErrorPage = getcwd(NULL, 0) + _error_page[1];

	if (_error_page[0] == statusCode)
	{
		if (!(checkFile(_defaultErrorPage) == 200))
			_defaultErrorPage = "";
	}
	else
		_defaultErrorPage = "";
}

/***************************************************************************************************/

// generate response header and return it
void Response::sendResponse(int statusCode, std::string finalPath)
{
	checkDefaultErrorPage(std::to_string(statusCode));
	if (statusCode == 301)
	{
		_response.first = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + finalPath + "\r\n\r\n";
		_response.second = "";
		return;
	}

	std::string contentType = getFileType(finalPath);

	if (contentType.empty() && _defaultErrorPage.empty())
		contentType = "text/plain";
	else if (contentType.empty() && !_defaultErrorPage.empty())
		contentType = getFileType(_defaultErrorPage);
	contentType = "Content-type: " + contentType + "\r\n";
	std::string Date = "Date: " + formatted_time() + "\r\n\r\n";

	if (_request.get_status() == 201)
		statusCode = 201;

	if (statusCode == 200 || statusCode == 201)
	{
		setSuccessResponse(statusCode, contentType, finalPath);
		_response.first += Date;
		_response.second = finalPath;
	}
	else
	{
		if (_defaultErrorPage != "")
			finalPath = _defaultErrorPage;
		setErrorResponse(contentType, statusCode, finalPath);
		_response.first += Date;
	}
}

/***************************************************************************************************/

void Response::setErrorResponse(std::string contentType, int statusCode, std::string finalPath)
{
	std::pair<std::string, int> message = statusMessage(statusCode);

	// std::cout << "default error page: " << finalPath << std::endl;
	_response.first = "HTTP/1.1 " + message.first + "\r\n";

	if (_defaultErrorPage != "")
	{
		_response.first += contentType + "Content-Length: " + fileSize(finalPath) + "\r\n";
		_response.second = finalPath;
	}
	else
	{
		_response.first += contentType + "Content-Length: " + std::to_string(message.second) + "\r\n";
		_response.second = message.first;
	}
}

/***************************************************************************************************/

std::vector<std::string> Response::getErrorPage()
{
	return _server._server_error_page;
}

/***************************************************************************************************/
void Response::CGI(std::string which_cgi)
{
	char *args[3];

	int fd;
	if ((fd = open("cgi.html", O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1)
	{
		close(fd);
		sendResponse(502, "");
		return;
	}
	char *tmp = getcwd(NULL, 0);
	std::string path = tmp + std::string("/cgi.html");
	free(tmp);
	setEnv();
	pid_t pid = fork();

	int status_execve = 0;
	int fd_body = open(_request.get_body().c_str(), O_RDONLY);

	if (pid == 0)
	{
		dup2(fd, 1);
		if (fd_body != -1)
			dup2(fd_body, 0);
		else
			close(0);

		args[0] = (char *)_cgi.c_str();
		args[1] = strdup(_finalPath.c_str());
		args[2] = NULL;

		if (execve(args[0], args, environ) < 0)
		{
			perror("execve");
			close(fd);
			status_execve = 1;
			exit(1);
		}
		exit(0);
	}
	else
	{
		int stat;
		waitpid(0, &stat, 0);
		close(fd);
		close(fd_body);
		if (status_execve != 1)
		{
			if (which_cgi == "php")
				parse_CGI_header(path);
			sendResponse(200, path);
		}
		else
			sendResponse(502, "");
	}
}

void Response::setEnv()
{
	if (_request.get_header("Cookie") != "")
		setenv("HTTP_COOKIE", _request.get_header("Cookie").c_str(), 1);
	if (_request.get_method() == "POST")
		if (_request.get_header("Content-type").empty())
			setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
	setenv("CONTENT_LENGTH", std::to_string(_request.get_content_length()).c_str(), 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("REQUEST_METHOD", _method.c_str(), 1);
	setenv("QUERY_STRING", _request.get_queryString().c_str(), 1);
	setenv("SCRIPT_FILENAME", _finalPath.c_str(), 1);
	setenv("SERVER_SOFTWARE", "ADA", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("REDIRECT_STATUS", "true", 1);
}

void Response::parse_CGI_header(std::string path)
{
	std::ifstream file(path);
	std::string line;
	bool isBody = false;

	while (std::getline(file, line))
	{
		if (!isBody)
		{
			size_t pos1 = line.find(":");
			size_t pos2 = line.find("\r");
			if (pos1 == std::string::npos)
			{
				isBody = true;
				continue;
			}
			std::string key = line.substr(0, pos1);
			line.erase(0, pos1 + 2);
			std::string value = line.substr(0, pos2);
			_cgiHeaders.insert(std::make_pair(key, value));
		}
		else
			_cgiBody += line;
	}

	file.close();
	std::ofstream file2(path);
	file2 << _cgiBody;
	file2.close();

	// print the content of the file
	// for (std::map<std::string, std::string>::iterator it = _cgiHeaders.begin(); it != _cgiHeaders.end(); ++it)
	// 	std::cout << it->first << ": " << it->second << std::endl;
	// std::cout << _cgiBody << std::endl;

	if (!get_header("Status").empty())
		this->_statusCode = get_header("Status");
	if (!get_header("Content-type").empty())
		this->_contentType = get_header("Content-type");
}

std::string Response::get_header(std::string headerName) const
{
	if (this->_cgiHeaders.find(headerName) == this->_cgiHeaders.end())
		return "";
	return this->_cgiHeaders.find(headerName)->second;
}

void Response::setSuccessResponse(int statusCode,std::string contentType, std::string finalPath)
{
    std::pair<std::string, int> message;
    message = (_statusCode != "") ? statusMessage(stoi(_statusCode)) : statusMessage(statusCode);
    
    _response.first = std::string("HTTP/1.1 ") + message.first + std::string("\r\n");

    if (get_header("Content-type") != "")
        _response.first += std::string("Content-type: ") + get_header("Content-type") + std::string("\r\n");
    else
        _response.first += contentType;
    _response.first += "Content-Length: " + fileSize(finalPath) + "\r\n";
    std::cout << "content-length: " << fileSize(finalPath) << std::endl;

    // add php cgi headers
    for (std::map<std::string, std::string>::iterator it = _cgiHeaders.begin(); it != _cgiHeaders.end(); ++it)
    {
        if (it->first == "Status" || it->first == "Content-type" || it->first == "Content-Lenght")
            continue;
        else
            _response.first += it->first + std::string(": ") + it->second + std::string("\r\n");
    }
}