/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 01:09:37 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/22 23:53:40 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_HPP
# define CONFIG_FILE_HPP
# include <vector>
# include <string>
# include <iostream>
# include <fstream>
#include <algorithm>
# include "server.hpp"


class parser_conf_file
{
    public:
        std::vector<parser_server>    _servers;

    public:
        parser_conf_file();
        parser_conf_file(std::string file_name);
        parser_conf_file(const parser_conf_file &src);
        parser_conf_file &operator=(const parser_conf_file &src);
        ~parser_conf_file();
        void parse(std::ifstream &file);
        void set_data_sever(std::ifstream &file);
        parser_location set_data_location(std::ifstream &file);
        bool is_number(std::string str);
        std::vector<std::string> split(std::string value, char c);
        void check_repetion_port_server(std::vector<parser_server> _servers);
        void check_attr_is_missing(std::vector<parser_server> &_servers);
        bool is_empty(std::string file_name);
        std::vector<std::string> get_data(std::string &line, std::string error, int len);
        bool validate_ip(std::string ip);

};


#endif