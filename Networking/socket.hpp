
#ifndef SOCKET_HPP
# define SOCKET_HPP
# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include "../Parser/config_file.hpp"
# include "unistd.h"
# include <fcntl.h>
# include <map>
# include "./data_socket.hpp"
# include <cstring> // for memset (ubuntu)
# include <signal.h> // signal (ubuntu)

class server_socket;

class Socket
{
    private:
        parser_server _which_server;
        std::vector<server_socket> _sock_vec;
        parser_conf_file _servers;
        int _socket_fd; // socket file descriptor
        int _port;
        std::string _host;
        struct sockaddr_in _address; // socket address
        fd_set _readfds; //read file descriptor set
        fd_set _readfdsCopy; // read file descriptor set copy
        fd_set _writefds; //write file descriptor set
        fd_set _writefdsCopy; //write file descriptor set copy
        void binding();
        void listening();
        void set_non_blocking(int new_socket);
        size_t _index;

    public:
        Socket();
        Socket(parser_conf_file &conf);
        ~Socket();
        void setPort(int port);
        void setHost(std::string host);
        void set_wich_server(parser_server &server); // set the server to send the response
        void create(int domain, int service, int protocol);
        void accepter();
        int _select();
        size_t getFileLength(std::string _fpath);
};


#endif