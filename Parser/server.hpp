/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 22:21:05 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/21 22:31:37 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <vector>
#include <string>
#include <iostream>

class parser_location
{
public:
    std::string _location_path;
    std::vector<std::string> _location_method;
    std::vector<std::string> _location_return;
    std::string _location_root;
    bool _location_auto_index;
    std::string _location_default;
    std::string _location_cgi;
    std::string _location_upload;

public:
    parser_location();
    parser_location(const parser_location &src);
    parser_location &operator=(const parser_location &src);
    ~parser_location();
};

class parser_server
{
public:
    int _server_port;
    std::string _server_host;
    std::string _server_name;
    std::vector<std::string> _server_error_page;
    long _server_body_limit;
    std::string _server_root;
    std::vector<parser_location> _server_location;

public:
    parser_server();
    parser_server(const parser_server &src);
    parser_server &operator=(const parser_server &src);
    ~parser_server();
};

#endif