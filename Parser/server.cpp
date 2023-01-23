/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 22:42:10 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/21 20:07:48 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
parser_server::parser_server() : _server_port(0),
                                 _server_host(""),
                                 _server_name(""),
                                 _server_error_page(),
                                 _server_body_limit(-1),
                                 _server_location()
{
}

parser_server::parser_server(const parser_server &src)
{
    *this = src;
}

parser_server &parser_server::operator=(const parser_server &src)
{
    _server_port = src._server_port;
    _server_host = src._server_host;
    _server_name = src._server_name;
    _server_error_page = src._server_error_page;
    _server_body_limit = src._server_body_limit;
    _server_location = src._server_location;
    return *this;
}

parser_server::~parser_server()
{
}

parser_location::parser_location() : _location_path(""),
                                     _location_method(),
                                     _location_return(),
                                     _location_root(""),
                                     _location_auto_index(false),
                                     _location_default(""),
                                     _location_cgi(""),
                                     _location_upload("")
{
}

parser_location::parser_location(const parser_location &src)
{
    *this = src;
}

parser_location &parser_location::operator=(const parser_location &src)
{
    _location_path = src._location_path;
    _location_method = src._location_method;
    _location_return = src._location_return;
    _location_root = src._location_root;
    _location_auto_index = src._location_auto_index;
    _location_default = src._location_default;
    _location_cgi = src._location_cgi;
    _location_upload = src._location_upload;
    return *this;
}

parser_location::~parser_location()
{
}
