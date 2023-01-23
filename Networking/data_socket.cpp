/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_socket.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:36:16 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/16 16:40:11 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "data_socket.hpp"

server_socket::server_socket(int kind, int socket, parser_server server) : _request(NULL),
										_response(NULL),
										 _read_size(0),
										 _file_lenght(0),
										 _file_sent(0),
										 _check_is_path(0)
{
	_kind = kind;
	_socket = socket;
	_server = server;
}

// server_socket::server_socket(server_socket const & src)
// {
// 	*this = src;
// }

// server_socket & server_socket::operator=(server_socket const & src)
// {
// 	if (this != &src)
// 	{
// 		_kind = src._kind;
// 		_socket = src._socket;
// 		_server = src._server;
// 		_request = src._request;
// 		_response = src._response;
// 		_res = src._res;
// 		_read_size = src._read_size;
// 		_file_fd = src._file_fd;
// 		_file_lenght = src._file_lenght;
// 		_file_sent = src._file_sent;
// 	}
// 	return (*this);
// }

server_socket::~server_socket()
{

}


int server_socket::get_kind()
{
	return _kind;
}

int server_socket::get_socket_fd()
{
	return _socket;
}

parser_server server_socket::get_server()
{
	return _server;
}

// void server_socket::parse_buffer_req(char buffer[1024])
// {
// 	_request->parse_buffer(buffer);
// }0

// void server_socket::set_data_for_response(parser_server server, Request request)
// {
// 	_response->set_data(server, request);
// }

// int server_socket::is_finish()
// {
// 	return _request->is_finish();
// }

void server_socket::set_server(parser_server server)
{
	_server = server;
}