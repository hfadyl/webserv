#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <fstream> // files
#include <ftw.h> // file tree walk (traverse (walk) a file tree)
#include "../Parser/config_file.hpp"
#include "../Parser/request.hpp"
#include "../Networking/socket.hpp"
#include "helpers.hpp"
#include <dirent.h> // directory
#include <string>

 extern char **environ;

class Response
{
private:
    std::string _directory;
    int _socket_fd;
    parser_server _server;
    Request _request;
    std::string _uri;
    std::string _method;
    parser_location _location;
    std::string _matchingLocation;
    std::string _finalPath;
    bool _sent;
    std::pair<std::string, std::string> _response;
    std::vector<std::string> _error_page;
    std::string _cgi;
    std::string _statusCode;
    std::string _contentType;
    std::map<std::string, std::string> _cgiHeaders;
    std::string _cgiBody;
    std::string _cgiPathResponse;
    std::string _defaultErrorPage;
    int _response_sent;


public:
    // canonical form
    Response(parser_server server, Request request, int socket_fd);
    Response & operator=(Response const &rhs);
    Response(Response const & src);
    ~Response();

    // intro to response
    void startResponse(parser_server server);

    // http methods
    void GET(DIR *dir);
    void POST(std::string pathType);
    void DELETE(std::string pathType_, DIR *dir);
    //
    bool isAllowedMethod(std::string method);

    // path/uri/location handling
    std::string matchingLocation(parser_server server);
    void handlePath(parser_server  server);
    void pathType();
    std::string longestPath(std::vector<std::string> strings, parser_server server);

    // directories and files
    void handleDirectory(DIR *dir);
    bool handleFile();
    bool isEmptyFolder(DIR *dir);
    bool isEmptyFolderDel(DIR *dir);
    bool checkFileAccess(std::string path);
    std::string fileContent(std::string path);
    int checkFile(std::string file);

    // check default error page
    void checkDefaultErrorPage(std::string statusCode);

    // autoindex
    void autoIndex();
    void autoIndexDirectory(DIR *dir, std::ofstream &file);

    // set and get response
    std::pair<std::string, std::string> getResponse();
    void sendResponse(int statusCode, std::string finalPath);
    void setErrorResponse(std::string, int, std::string);
    void setSuccessResponse(int ,std::string , std::string );

    // CGI
    void CGI(std::string);
    void CGI_exec(std::string path);
    void setEnv();
    void parse_CGI_header(std::string);

    std::vector<std::string> getErrorPage();
    std::string get_header(std::string headerName) const;
};

#endif