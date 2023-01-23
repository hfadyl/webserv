#include "request.hpp"

void Request::parse_chunck(char *buffer, long size)
{
    std::string buff(buffer, size);
    size_t chunk_found = 0;
    bool completed = false;
    _data += get_buffer(buff, completed);
    _data = clean_hexa(_data, chunk_found);
    _data = clean_hexa(_data, chunk_found); // in case we have a second chunk on the same buffer (eg. small data)
    if (!chunk_found)
        return;
    std::string chunk = _data.substr(0, chunk_found);
    write(_fd, chunk.c_str(), chunk.size());
    _bodySize += chunk.size();
    _data.erase(0, chunk_found);
    if (completed)
        _finish = 1;
}

// ----------------[pos]\r\n10000\r\n[i]-------------
std::string Request::clean_hexa(std::string buff, size_t &chunk_found)
{
    size_t pos = buff.find("\r\n");
    while (pos != std::string::npos)
    {
        size_t i = pos + 2;
        while (is_hexa(buff[i]))
            i++;
        if (i > pos + 2 && buff[i] == '\r' && buff[i + 1] == '\n')
        {
            // std::cerr << _chunk++ << " => [0x" << buff.substr(pos + 2, i - pos - 2) << "]" << std::endl;
            chunk_found = pos;
            buff.erase(pos, i - pos + 2);
            break;
        }
        pos = buff.find("\r\n", pos + 2);
    }
    return buff;
}

std::string Request::get_buffer(std::string buff, bool &completed)
{
    size_t pos = buff.find("\r\n0\r\n\r\n");
    if (pos != std::string::npos)
    {
        completed = true;
        return buff.substr(0, pos + 5);
    }
    return buff;
}