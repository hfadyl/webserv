/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config_file.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hfadyl <hfadyl@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 01:10:46 by hfadyl            #+#    #+#             */
/*   Updated: 2022/06/23 00:09:53 by hfadyl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config_file.hpp"

parser_conf_file::parser_conf_file()
{
}

parser_conf_file::parser_conf_file(std::string file_name)
{
    std::ifstream file;
    file.open(file_name, std::ios::in);
    if (is_empty(file_name))
        throw std::runtime_error("Error : file is empty, or has just new lines or comment");
    if (file.is_open())
        this->parse(file);
    else
        throw std::runtime_error("Error: Can't open file or doesn't exist: ");
    file.close();
}

bool parser_conf_file::is_empty(std::string file_name)
{
    int ret = 0;
    std::ifstream file;
    file.open(file_name, std::ios::in);
    if (!file.is_open())
        throw std::runtime_error("Error: Can't open file or doesn't exist: ");
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        file.close();
        return true;
    }
    std::string line;
    while (std::getline(file, line))
    {
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.find("server{") != std::string::npos)
            ret = 1;
    }
    if (ret == 0)
    {
        file.close();
        return true;
    }
    file.close();
    return false;
}

parser_conf_file::parser_conf_file(const parser_conf_file &src)
{
    *this = src;
}

parser_conf_file &parser_conf_file::operator=(const parser_conf_file &src)
{
    this->_servers = src._servers;
    return *this;
}

parser_conf_file::~parser_conf_file()
{
}

void parser_conf_file::parse(std::ifstream &file)
{
    std::string line;
    while (std::getline(file, line))
    {
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.find("server{") != std::string::npos)
        {
            this->set_data_sever(file);
        }
    }
}

bool parser_conf_file::is_number(std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

std::vector<std::string> parser_conf_file::split(std::string value, char c)
{
    size_t i = 0;
    std::string str = value;
    std::vector<std::string> vec;
    while ((i = str.find(c)) != std::string::npos)
    {
        if (str.substr(0, i).length() > 0)
            vec.push_back(str.substr(0, i));
        str = str.substr(i + 1);
    }
    if (str.length() > 0)
        vec.push_back(str);
    return vec;
}

void parser_conf_file::check_repetion_port_server(std::vector<parser_server> _servers)
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        for (size_t j = i + 1; j < _servers.size(); j++)
        {
            if ((_servers[i]._server_port == _servers[j]._server_port) && (_servers[i]._server_name == _servers[j]._server_name))
                throw std::runtime_error("Error: You Can't use a same Port and server name in two servers");
        }
    }
}

void parser_conf_file::check_attr_is_missing(std::vector<parser_server> &_servers)
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        if (_servers[i]._server_port == 0)
        {
            throw std::runtime_error("Error: You must set a port for server");
        }
        if (_servers[i]._server_host == "")
        {
            throw std::runtime_error("Error: You must set a host for server");
        }
        if (_servers[i]._server_location.size() == 0)
        {
            throw std::runtime_error("Error : must provide a location");
        }
    }
}

std::vector<std::string> parser_conf_file::get_data(std::string &line, std::string error, int len)
{
    int i = line.find("=");
    std::string value = line.substr(i + 1);
    std::vector<std::string> vec = split(value, ' ');
    if (len != -1 && vec.size() > (size_t)len)
        throw std::runtime_error(error);
    if (vec.size() == 0)
        throw std::runtime_error(error);
    return vec;
}

bool parser_conf_file::validate_ip(std::string ip)
{
    // split the string into tokens
    std::vector<std::string> list = split(ip, '.');

    // if the token size is not equal to four
    if (list.size() != 4)
        return false;

    // validate each token
    for (int i = 0; i < 4; i++)
    {
        // if the token is not a number
        if (list[i].length() > 3 || !is_number(list[i]))
            return false;
        // if the token is not in the range 0-255
        if (stoi(list[i]) < 0 || stoi(list[i]) > 255)
            return false;
    }

    return true;
}

void parser_conf_file::set_data_sever(std::ifstream &file)
{
    std::string line;
    parser_server server;
    size_t i = 0;
    while (std::getline(file, line))
    {
        if (line.find("#") != std::string::npos)
            throw std::runtime_error("Error : forbeiden to have a comment inside the brackets.");
        std::string tmpline = line;
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.empty())
            continue;
        if (line.find("}") != std::string::npos)
        {
            _servers.push_back(server);
            // check if port and server_name has the same value of servers
            check_repetion_port_server(_servers);
            // check attr if missing replace with it's default
            check_attr_is_missing(_servers);
            return;
        }
        if ((i = line.find("=")) == std::string::npos)
            throw std::runtime_error("Error : must be a '=' between key and value.");
        std::string key = line.substr(0, i);
        std::string value = line.substr(i + 1);
        if (value.empty() || key.empty())
            throw std::runtime_error("Error : check config file key or value is missing");
        if (key == "port")
        {
            if (!is_number(value))
                throw std::runtime_error("Error: port must be a number");
            if (server._server_port != 0)
                throw std::runtime_error("Error : Duplicate port not allowed!");
            server._server_port = std::stoi(get_data(tmpline, "Error : server port must be just one parameter like : 8000", 1)[0]);
        }
        else if (key == "host")
        {
            if (server._server_host != "")
                throw std::runtime_error("Error : Duplicate host not allawed!");
            server._server_host = get_data(tmpline, "Error : host must be something like '127.0.0.1' ", 1)[0];
            if (!validate_ip(server._server_host))
                throw std::runtime_error("Error : host Not Valid.");
        }
        else if (key == "server_name")
        {
            if (server._server_name.size() != 0)
                throw std::runtime_error("Error : Duplicate server name not allawed!");
            server._server_name = get_data(tmpline, "Error : server name must be something like 'example.com' ", 1)[0];
        }
        else if (key == "error_page")
        {
            if (server._server_error_page.size() != 0)
                throw std::runtime_error("Error : Duplicate error page not allawed!");
            server._server_error_page = get_data(tmpline, "Error : Error page must be something like '404 /defaultPages/' ", 2);
            if (!is_number(server._server_error_page[0]))
                throw std::runtime_error("Error : Error page must be a number");
            if (server._server_error_page.size() != 2)
                throw std::runtime_error("Error : Error page must be something like '404 /defaultPages/' ");
        }
        else if (key == "body_size_limit")
        {
            if (!is_number(value))
                throw std::runtime_error("Error: body size limit must be a number");
            if (server._server_body_limit != -1)
                throw std::runtime_error("Error : Duplicate body size limit not allawed!");
            server._server_body_limit = std::stoi(get_data(tmpline, "Error : server size limit must be just one parameter like : 10000", 1)[0]);
        }
        else if (key == "root")
        {
            if (server._server_root != "")
                throw std::runtime_error("Error : Duplicate server root not allawed!");
            server._server_root = get_data(tmpline, "Error : root must be something like '/example/example..'", 1)[0];
            if (server._server_root[server._server_root.length() - 1] != '/')
                server._server_root += '/';
        }
        else if (key == "location")
        {
            parser_location location = set_data_location(file);
            tmpline = tmpline.erase(tmpline.find_last_not_of('[') + 1);
            value = get_data(tmpline, "Error : location path must be something like '/USERS' ", 1)[0];
            if ((value[value.length() - 2] == '/' && value.length() > 2) || (value[0] == '/' && value[1] == '/'))
                throw std::runtime_error("Error : location path must not have a bad format");
            if (value[value.length() - 1] == '/' && value.length() > 1)
                value = value.substr(0, value.length() - 1);
            if (value[0] != '/')
                value = "/" + value;
            location._location_path = value;
            if (location._location_root == "")
                throw std::runtime_error("Error : you must set a root for location");
            server._server_location.push_back(location);
        }
        else
            throw std::runtime_error("Error : Key not defined.");
    }
}

parser_location parser_conf_file::set_data_location(std::ifstream &file)
{
    std::string line;
    parser_location location;
    size_t i = 0;
    while (std::getline(file, line))
    {
        std::string tmpline = line;
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.empty())
            continue;
        if (line.find("]") != std::string::npos)
        {
            return location;
        }
        if ((i = line.find("=")) == std::string::npos)
            throw std::runtime_error("Error : must be a '=' between key and value.");
        std::string key = line.substr(0, i);
        std::string value = line.substr(i + 1);
        if (value.empty() || key.empty())
            throw std::runtime_error("Error : check config file key or value is missing");
        if (key == "method")
            location._location_method = get_data(tmpline, "Error : mehtods must be just 3 [Get, POST, DELETE]", 3);
        else if (key == "redirect")
        {
            location._location_return = get_data(tmpline, "Error : redirect must be something like '302 https://example.com' ", 2);
            if (location._location_return.size() != 2 || !is_number(location._location_return[0]))
                throw std::runtime_error("Error : redirect must be something like '302 https://example.com' ");
            if (location._location_return[0] != "301")
                throw std::runtime_error("Error : bad redirect code");
        }
        else if (key == "root")
        {
            location._location_root = get_data(tmpline, "Error : root must be something like '/Users/hfadyl/Desktop ' ", 1)[0];
            if (location._location_root[location._location_root.length() - 1] != '/')
                location._location_root += '/';
        }
        else if (key == "autoindex")
        {
            i = tmpline.find("=");
            value = tmpline.substr(i + 1);
            std::vector<std::string> tmpvec = split(value, ' ');
            if (tmpvec.size() > 1)
                throw std::runtime_error("Error : autoindex must be 'on' or 'off' ");
            if (tmpvec[0] != "on" && tmpvec[0] != "off")
                throw std::runtime_error("Error : autoindex must be 'on' or 'off' ");
            if (tmpvec[0] == "on")
                location._location_auto_index = true;
        }
        else if (key == "default")
        {
            location._location_default = get_data(tmpline, "Error : default must be something like '/example.html' ", 1)[0];
            if (location._location_default[0] == '/')
                location._location_default = location._location_default.substr(1);
        }
        else if (key == "cgi")
        {
            location._location_cgi = get_data(tmpline, "Error : You must provide a path to cgi.", 1)[0];
            if (location._location_cgi != "php" && location._location_cgi != "python")
                throw std::runtime_error("Error : cgi must be 'php' or 'python' ");
        }
        else if (key == "upload")
        {
            location._location_upload = get_data(tmpline, "Error : You must provide a path to upload", 1)[0];
            if (location._location_upload[location._location_upload.length() - 1] == '/')
                location._location_upload = location._location_upload.substr(0, location._location_upload.length() - 1);
            if (location._location_upload[0] == '/')
                location._location_upload = location._location_upload.substr(1);
        }
        else
            throw std::runtime_error("Error : Key not defined.");
    }
    return location;
}
