#include "request.hpp"

Request::Request() {}

Request::~Request()
{
    _headers.clear();
    if (_fd)
        close(_fd);
}

Request::Request(parser_conf_file conf) : _method(""),
                                          _path(""),
                                          _version(""),
                                          _host(""),
                                          _port(""),
                                          _contentLength(0),
                                          _acceptEncoding(),
                                          _headers(),
                                          _finish(-1),
                                          _status(200),
                                          _bodySize(0),
                                          _headerIsSet(false),
                                          _conf(conf),
                                          _bodySizeLimit(-1),
                                          _queryString(""),
                                          _chunk(0),
                                          _data(""),
                                          _is_chunked(false)
{
    _body = "tmp" + random_string(10);
}

void Request::parse_buffer(char buffer[BUFFER_SIZE], long size)
{
    if (!_headerIsSet) // if there is no header yet/
    {
        std::string line(buffer, size);         // copy the buffer to a string with size
        size_t pos = line.find("\r\n\r\n") + 2; // body start position
        if ((pos - 2) == std::string::npos)
            pos = line.size();
        size = line.size() - pos;                     // body size
        if (!this->parse_header(line.substr(0, pos))) // parse header without \r\n\r\n
        {
            _finish = 1;
            return;
        }
        line.erase(0, pos); // -->headers...\r\n<---[\r\n...body]
        _headerIsSet = true;
        if (!set_body_size_limit())
            return;
        matchingLocation(_which_server);
        if (!_which_location._location_upload.empty())
            _body = _which_location._location_upload + random_string(10);
        if (get_header("Content-Type").empty())
            _finish = 1;
        else
        {
            _body += get_file_ext(get_header("Content-Type"));
            _fd = open(_body.c_str(), O_RDWR | O_APPEND | O_CREAT, 0777);
            if (_is_chunked)
                this->parse_body(buffer + pos, size); // get chuncked body
            else
                this->parse_body(buffer + pos + 2, size - 2); // get body
        }
        return;
    }
    if (size > 0)
        this->parse_body(buffer, size); // get rest of body
}

bool Request::parse_header(std::string buffer)
{
    std::string requestLine = buffer.substr(0, buffer.find("\r\n"));
    size_t pos = requestLine.find(" ");
    if ((_status = set_method(requestLine.substr(0, pos))) != 200)
        return false;
    requestLine.erase(0, pos + 1);
    pos = requestLine.find(" ");
    if ((_status = set_path(requestLine.substr(0, pos))) != 200)
        return false;
    requestLine.erase(0, pos + 1);
    if ((_status = set_version(requestLine)) != 200)
        return false;

    while (buffer.find("\r\n") != std::string::npos)
    {
        buffer.erase(0, buffer.find("\r\n") + 2);
        std::string headerLine = buffer.substr(0, buffer.find("\r\n"));
        pos = headerLine.find(":");
        std::string key = headerLine.substr(0, pos);
        headerLine.erase(0, pos + 2);
        std::string value = headerLine;
        if (key.empty() || value.empty())
            continue;
        if ((_status = set_header(key, value)) != 200)
            return false;
    }
    _is_chunked = _headers["Transfer-Encoding"] == "chunked";
    // this->show_headers();
    return true;
}

void Request::parse_body(char *buffer, long size)
{
    if (_is_chunked)
        parse_chunck(buffer, size);
    else
    {
        write(_fd, buffer, size);
        _bodySize += size;
    }
    if (_bodySize >= _contentLength)
    {
        _finish = 1;
        _status = 201;
        // std::cout << GREEN << "[" << _body << " file is uploaded successfully" << std::endl;
        // std::cout << "_contentLength " << _contentLength << " _bodySize " << (int)_bodySize << DEFAULT << std::endl;
    }
}

//----------- Setters -------------//
int Request::set_header(std::string key, std::string value)
{
    if (key == "Host")
        return set_host_port(value);
    if (key == "Content-Length")
        return set_content_length(value);
    if (key == "Accept-Encoding")
    {
        std::stringstream ss(value);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            if (token[0] == ' ') // remove first space
                token.erase(0, 1);
            _acceptEncoding.push_back(token);
        }
    }
    else
        _headers.insert(std::make_pair(key, value));
    return 200;
}
int Request::set_method(std::string method)
{
    if (method != "GET" && method != "POST" && method != "DELETE")
        return 501;
    _method = method;
    return 200;
}
int Request::set_path(std::string path)
{
    // path too long
    if (path.size() > 2048)
        return 414;
    size_t pos = path.find("?");
    if (pos != std::string::npos)
    {
        _queryString = path.substr(pos + 1);
        path.erase(pos);
    }
    _path = path;
    return 200;
}
int Request::set_version(std::string version)
{
    if (version != "HTTP/1.1" && version != "HTTP/1.0")
        return 505;
    _version = version;
    return 200;
}
int Request::set_host_port(std::string host)
{
    _host = host.substr(0, host.find(":"));
    _port = host.substr(host.find(":") + 1, host.size());
    if (_port.empty()) // if no port set, set default port
        _port = "80";
    return 200;
}
int Request::set_content_length(std::string size)
{
    _contentLength = (size_t)stoi(size);
    return 200;
}
bool Request::set_body_size_limit()
{
    if (_bodySizeLimit == -1)
    {
        for (size_t i = 0; i < _conf._servers.size(); i++)
        {
            if (_host == _conf._servers[i]._server_host && stoi(_port) == _conf._servers[i]._server_port)
            {
                _bodySizeLimit = _conf._servers[i]._server_body_limit;
                _which_server = _conf._servers[i];
                break;
            }
            else if (_host == _conf._servers[i]._server_name && stoi(_port) == _conf._servers[i]._server_port)
            {
                _bodySizeLimit = _conf._servers[i]._server_body_limit;
                _which_server = _conf._servers[i];
                break;
            }
        }
    }
    if (_bodySizeLimit != -1 && _bodySizeLimit < (long)_contentLength)
    {
        _finish = 1;
        _status = 413;
        return false;
    }
    return true;
}

std::string Request::matchingLocation(parser_server server)
{
    std::vector<std::string> matches;

    for (size_t i = 0; i < server._server_location.size(); i++)
        if ((findLocationPathInUri(_path, server._server_location[i]._location_path)))
            matches.push_back(server._server_location[i]._location_path);

    if (matches.empty() || (matches.size() == 1 && matches[0] != "/"))
        return ("no matching location");
    return longestPath(matches, server);
}
bool Request::findLocationPathInUri(std::string uri, std::string path)
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
std::string Request::longestPath(std::vector<std::string> strings, parser_server server)
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
            this->_which_location = server._server_location[i];

    return max_match;
}

//----------- Getters -------------//
int Request::is_finish() const
{
    return this->_finish; // return -1 if request not finish and 0 if finish
}
int Request::get_status() const
{
    return this->_status;
}
std::string Request::get_method() const
{
    return this->_method;
}
std::string Request::get_path() const
{
    return this->_path;
}
std::string Request::get_version() const
{
    return this->_version;
}
std::string Request::get_host() const
{
    return this->_host;
}
std::string Request::get_port() const
{
    return this->_port;
}
std::string Request::get_body() const
{
    char *tmp = getcwd(NULL, 0);
    std::string fullPath = tmp;
    free(tmp);
    return fullPath + "/" + this->_body;
}
std::string Request::get_queryString() const
{
    return this->_queryString;
}
size_t Request::get_body_size() const
{
    return this->_bodySize;
}
size_t Request::get_content_length() const
{
    return this->_contentLength;
}
std::string Request::get_header(std::string headerName) const
{
    if (this->_headers.find(headerName) == this->_headers.end())
        return "";
    return this->_headers.find(headerName)->second;
}
std::vector<std::string> Request::get_accept_encoding() const
{
    return this->_acceptEncoding;
}

bool Request::is_number(const std::string &str) const
{
    // is str is number
    return !str.empty() &&
           (str.find_first_not_of("0123456789") == std::string::npos);
}
std::vector<std::string> Request::split(std::string value, std::string delim) const
{
    std::vector<std::string> tokens;
    char *ptr = strtok((char *)value.c_str(), delim.c_str());
    while (ptr != NULL)
    {
        tokens.push_back(ptr);
        ptr = strtok(NULL, delim.c_str());
    }
    return tokens;
}
std::string Request::random_string(int length) const
{
    char alphaNum[62] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                         'h', 'i', 'j', 'k', 'l', 'm', 'n',
                         'o', 'p', 'q', 'r', 's', 't', 'u',
                         'v', 'w', 'x', 'y', 'z', '0', '1',
                         '2', '3', '4', '5', '6', '7', '8',
                         '9', 'A', 'B', 'C', 'D', 'E', 'F',
                         'G', 'H', 'I', 'J', 'K', 'L', 'M',
                         'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                         'U', 'V', 'W', 'X', 'Y', 'Z'};
    std::string result = "";
    for (int i = 0; i < length; i++)
        result += alphaNum[rand() % 62];
    return "/" + result;
}
bool Request::is_hexa(char c) const
{
    if (c >= '0' && c <= '9')
        return true;
    if (c >= 'a' && c <= 'f')
        return true;
    if (c >= 'A' && c <= 'F')
        return true;
    return false;
}
void Request::show_headers()
{
    std::cout << GREEN << "\n\n======================= [HEADER BEGIN] =======================" << std::endl;
    std::cout << _method << " " << _path << " " << _version << std::endl;
    std::cout << "Host: [" << _host << "]" << std::endl;
    std::cout << "Post: [" << _port << "]" << std::endl;
    std::cout << "Content-Length: [" << _contentLength << "]" << std::endl;
    std::cout << "Query String: [" << _queryString << "]" << std::endl;
    if (_acceptEncoding.size() > 0)
    {
        std::cout << "Accept-Encoding: ";
        for (std::vector<std::string>::iterator it = _acceptEncoding.begin(); it != _acceptEncoding.end(); ++it)
            std::cout << "[" << *it << "] ";
        std::cout << std::endl;
    }
    // print all other headers
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;

    std::cout << "=======================  [HEADER END]  =======================\n"
              << DEFAULT
              << std::endl;

    if (_status != 200)
        std::cout << RED << "Status: " << _status << DEFAULT << std::endl;
}

std::string Request::get_file_ext(std::string res)
{
    if (!res.empty())
    {
        if ("text/html" == res)
            return (".html");
        else if ("text/css" == res)
            return (".css");
        else if ("text/xml" == res || "application/xml" == res)
            return (".xml");
        else if ("text/csv" == res)
            return (".csv");
        else if ("image/gif" == res)
            return (".gif");
        else if ("image/x-icon" == res)
            return (".ico");
        else if ("image/jpeg" == res)
            return (".jpeg");
        else if ("application/javascript" == res)
            return (".js");
        else if ("application/json" == res)
            return (".json");
        else if ("image/png" == res)
            return (".png");
        else if ("application/pdf" == res)
            return (".pdf");
        else if ("image/svg+xml" == res)
            return (".svg");
        else if ("text/plain" == res)
            return (".txt");
        else if ("application/atom+xml" == res)
            return (".atom");
        else if ("application/rss+xml" == res)
            return (".rss");
        else if ("image/webp" == res)
            return (".webp");
        else if ("video/3gpp" == res)
            return (".3gpp");
        else if ("video/3gpp" == res)
            return (".3gp");
        else if ("video/mp2t" == res)
            return (".ts");
        else if ("video/mp4" == res)
            return (".mp4");
        else if ("video/mpeg" == res)
            return (".mpeg");
        else if ("video/mpeg" == res)
            return (".mpg");
        else if ("video/quicktime" == res)
            return (".mov");
        else if ("video/webm" == res)
            return (".webm");
        else if ("video/x-flv" == res)
            return (".flv");
        else if ("video/x-m4v" == res)
            return (".m4v");
        else if ("video/x-mng" == res)
            return (".mng");
        else if ("video/x-ms-asf" == res)
            return (".asx");
        else if ("video/x-ms-asf" == res)
            return (".asf");
        else if ("application/javascript" == res)
            return (".js");
        else if ("video/x-ms-wmv" == res)
            return (".wmv");
        else if ("video/x-msvideo" == res)
            return (".avi");
        else if ("audio/midi" == res)
            return (".mid");
        else if ("audio/midi" == res)
            return (".midi");
        else if ("audio/midi" == res)
            return (".kar");
        else if ("audio/mpeg" == res)
            return (".mp3");
        else if ("audio/ogg" == res)
            return (".ogg");
        else if ("audio/x-m4a" == res)
            return (".m4a");
        else if ("audio/x-realaudio" == res)
            return (".ra");
        else if ("application/x-perl" == res)
            return (".pl");
        else if ("application/x-python" == res)
            return (".py");
        else if ("application/x-php" == res)
            return (".php");
        else if ("application/x-c++" == res)
            return (".cpp");
        else if ("application/x-c" == res)
            return (".c");
        else if ("application/zip" == res)
            return (".zip");
        else if ("application/x-rar-compressed" == res)
            return (".rar");
    }
    return "";
}