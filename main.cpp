/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 01:30:20 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/16 17:29:13 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser/config_file.hpp"
#include "Networking/socket.hpp"

int main(int argc, char **argv)
{
    try
    {
        if (argc == 2)
        {
            parser_conf_file conf(argv[1]);
            Socket Socket(conf);
            std::vector<int> vector_port;
            for (size_t i = 0; i < conf._servers.size(); i++)
            {
                if (find(vector_port.begin(), vector_port.end(), conf._servers[i]._server_port) == vector_port.end())
                {
                    vector_port.push_back(conf._servers[i]._server_port);
                    Socket.setPort(conf._servers[i]._server_port);
                    Socket.setHost(conf._servers[i]._server_host);
                    Socket.set_wich_server(conf._servers[i]);
                    Socket.create(AF_INET, SOCK_STREAM, 0);
                }
            }
            Socket.accepter();
        }
        else
        {
            throw std::runtime_error("Usage: ./webserv <config_file>");
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
