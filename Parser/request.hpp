#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "config_file.hpp"

#define BUFFER_SIZE 1024
#define BLUE "\e[1;34m"
#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define DEFAULT "\e[0m"

class Request
{
private:
    std::string _body;    // body path
    std::string _method;  // GET, POST, PUT, DELETE
    std::string _path;    // /index.html
    std::string _version; // HTTP/1.1
    std::string _host;    // 127.0.0.1
    std::string _port;    // 80
    size_t _contentLength;
    std::vector<std::string> _acceptEncoding;    // gzip, deflate, br
    std::map<std::string, std::string> _headers; // key: value of the header (key: value)
    int _finish;                                 // -1 if request not finish and 0 if finish
    int _status;
    size_t _bodySize;
    bool _headerIsSet;
    parser_conf_file _conf;
    long _bodySizeLimit;
    std::string _queryString;
    int _chunk;
    std::string _data;
    parser_server _which_server;
    parser_location _which_location;
    int _fd;
    bool _is_chunked;

    bool parse_header(std::string buffer);
    void parse_body(char *buffer, long size);
    void parse_chunck(char *buffer, long size);

    int set_header(std::string key, std::string value);
    int set_method(std::string method);
    int set_path(std::string path);
    int set_version(std::string version);
    int set_host_port(std::string host);
    int set_content_length(std::string size);
    bool set_body_size_limit();
    std::string matchingLocation(parser_server server);
    bool findLocationPathInUri(std::string uri, std::string path);
    std::string longestPath(std::vector<std::string> strings, parser_server server);

    bool validate_ip(std::string ip) const;
    bool is_number(const std::string &str) const;
    std::vector<std::string> split(std::string value, std::string delim) const;
    std::string random_string(int length) const;

    bool is_hexa(char c) const;
    void show_headers();
    std::string get_buffer(std::string buff, bool &completed);
    std::string clean_hexa(std::string buff, size_t &chunk_found);
    std::string get_file_ext(std::string res);

public:
    Request();
    Request(parser_conf_file conf);
    ~Request();
    // Request(const Request &src);
    // Request &operator=(const Request &src);

    void parse_buffer(char buffer[BUFFER_SIZE], long size);
    int is_finish() const;

    std::string get_method() const;
    std::string get_version() const;
    std::string get_path() const;
    std::string get_host() const;
    std::string get_port() const;
    std::string get_body() const;
    std::string get_queryString() const;
    size_t get_body_size() const;
    size_t get_content_length() const;
    std::string get_header(std::string headerName) const;
    std::vector<std::string> get_accept_encoding() const;
    int get_status() const;
};

#endif