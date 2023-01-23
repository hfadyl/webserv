/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_socket.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:34:47 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/16 19:48:58 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATA_SOCKET_HPP
#define DATA_SOCKET_HPP
# include "../Response/response.hpp"

class Response;

class server_socket
{
private:
    int _kind; // 0 for server, 1 for client
    int _socket;
    parser_server _server;

public:
    server_socket(int kind, int socket, parser_server server);
    // server_socket(server_socket const & src);
    // server_socket & operator=(server_socket const & src);
    ~server_socket();
    Request *_request;
    Response *_response;
    int _res;
    char buffer[1024];
    int get_kind();
    int get_socket_fd();
    // void parse_buffer_req(char buffer[1024], size_t size);
    // int is_finish();
    parser_server get_server();
    void set_server(parser_server server);


    int _read_size;
    int _file_fd;
    char _read_buffer[1024];
    size_t _file_lenght;
    size_t _file_sent;
    int _check_is_path;
};

#endif