#include "socket.hpp"

Socket::Socket(parser_conf_file &conf)
{
	_servers = conf;
	_index = 0;
	FD_ZERO(&_readfds);
	FD_ZERO(&_readfdsCopy);
	FD_ZERO(&_writefds);
	FD_ZERO(&_writefdsCopy);
}

Socket::~Socket()
{
}

void Socket::setPort(int port)
{
	_port = port;
}

void Socket::setHost(std::string host)
{
	_host = host;
}

/*
	domain (family of socket) :
		- AF_UNIX (local communication)
		- PF_XXX : PF_INET (protocol family internet IPv4), PF_INET6 (protocol family internet IPv6)
		- AF_XX : AF_INET (address family internet IPv4), AF_INET6 (address family internet IPv6)
			[(connect with different machine and inside same machine), (typically used)]

	service:
		SOCK_STREAM -> TCP (Transmission Control Protocol)
		- reliable byte-stream channel (in order, all arrive, no duplicates)
		- connection-oriented
		- bidirectional

		SOCK_DGRAM -> UDP (User Datagram Protocol)
		- out of order, duplicate possible.
		- connectionless (faster).

	protocol:
		Network protocols are formal standards and policies comprised of rules and formats that define the communication between
		two or more devices over the network.
		(defines when there is any additional protocol, Otherwise define it as 0)
		[cat /etc/protocols]
*/

void Socket::create(int domain, int service, int protocol)
{
	// socket function returns -1 on failure
	if ((_socket_fd = socket(domain, service, protocol)) < 1)
	{
		throw std::runtime_error("Error : Failed to create socket...");
	}
	set_non_blocking(_socket_fd);
	// setsockopt to allow multiple connections, means that the socket can be reused(to not face the failure to bind)
	int opt = 1;
	if ((setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0)
		throw std::runtime_error("Error : Failed to set socket option...");
	FD_SET(_socket_fd, &_readfds);
	// _socket_fd_vector.push_back(_socket_fd);
	// _server_socket_map.insert(std::pair<int, server_socket>(_socket_fd, server_socket(0, _which_server)));
	_sock_vec.push_back(server_socket(0, _socket_fd, _which_server));
	_index += 1;
	_address.sin_family = domain;
	// NOTE : socket call does not specify where data will be coming from, nor where it willbe going
	// to - it just creates the interface!
	binding();
	listening();
}

void Socket::binding()
{

	/*
		- Server and client will start interfacing with each other after the bind function and it is
		the most important session.
		- Sockets don't have a complete address at the beginning to start data transfering,
		so we bind the socket to a port .
	*/

	/*
		(struct sockaddr *)&address ==> taking the value of what is stored in addressed
		and converting it to a struct of  struct sockaddr *
		so it's a pointer to a memory location that holds this sort of struct and has
		has the value of address in it.
	*/
	// htons : Translate an unsigned short integer into network byte order
	_address.sin_port = htons(_port);
	// permit any incoming IP address, any client request
	_address.sin_addr.s_addr = inet_addr(_host.c_str()); // inet_addr("127.0.0.1")
	if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		throw std::runtime_error("Error : Failed to bind a socket...");
	}
	/*
		NOTE: the file name referred to in sockaddress. sun_path is created as a socket in the system file name space.
		the caller must have write permission in the directory whereaddr.sun_path is created.
		hte file should be deleted by the caller when it is no longer needed
   */
}

void Socket::listening()
{
	/*
		the process of converting unconnected socket into a passive socket, indicating that the kernal should accept
		incoming connection requests directed to this socket.
	*/
	if (listen(_socket_fd, SOMAXCONN) < 0)
		throw std::runtime_error("Error : Failed to listen for a request...");
	/*
		NOTES:
			- listen() is non-blocking : returns immediatly.
			- listen() is used by the server only as a way to get new sockets.
	*/
}

/*
	the server gets a socket for an incoming client connection by calling accept():
		Transition of the connection request from listen() method to an actuel socket. (accept the connection request).
		after accepting the request, the data be finally transfered between the nodes.
*/

/*
	With select(), instead of having a process for each request, there is usually only one process that "multi-plexes"
	all requests, servicing each request as much as it can.
	So one main advantage of using select() is that your server will only require a single process to handle all requests.
	Thus, your server will not need shared memory or synchronization primitives for different 'tasks' to communicate.
*/

// check non-blocking socket
void Socket::set_non_blocking(int new_socket)
{
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		close(new_socket);
		perror("Error : Failed in non blocking...");
	}
}

void Socket::set_wich_server(parser_server &server)
{
	_which_server = server;
}

size_t Socket::getFileLength(std::string _fpath)
{
	std::ifstream _file;
	size_t length;

	_file.open(_fpath.c_str(), std::ios::binary);
	_file.seekg(0, std::ios::end);
	length = _file.tellg();
	_file.close();
	return (length);
}

void Socket::accepter()
{
	int new_socket, activity;
	socklen_t addrlen = sizeof(_address);
	// signal to ignore a signal if send or recv is interrupted
	signal(SIGPIPE, SIG_IGN);
	std::string ret;
	std::cout << YELLOW << "***************************** Waiting for Connection... *****************************" << DEFAULT << std::endl;
	while (true)
	{
		ret= "";
		_readfdsCopy = _readfds;
		_writefdsCopy = _writefds;
		if ((activity = select(((_sock_vec.rbegin()->get_socket_fd()) + 1), &_readfdsCopy, &_writefdsCopy, NULL, NULL)) < 0)
			perror("Error : Failed to select...");
		for (size_t i = 0; i < _sock_vec.size(); i++)
		{
			if (FD_ISSET(_sock_vec[i].get_socket_fd(), &_readfdsCopy))
			{
				if (_sock_vec[i].get_kind() == 0)
				{
					if ((new_socket = accept(_sock_vec[i].get_socket_fd(), (struct sockaddr *)&_address, (socklen_t *)&addrlen)) < 0)
					{
						perror("Error : Failed to accept...");
						continue;
					}
					set_non_blocking(new_socket);
					_sock_vec.push_back(server_socket(1, new_socket, _sock_vec[i].get_server()));
					FD_SET(new_socket, &_readfds);
				}
				else
				{
					long t;
					memset(_sock_vec[i].buffer, 0, sizeof(_sock_vec[i].buffer));
					if ((t = recv(_sock_vec[i].get_socket_fd(), _sock_vec[i].buffer, sizeof(_sock_vec[i].buffer), 0)) <= 0)
					{
						FD_CLR(_sock_vec[i].get_socket_fd(), &_readfds);
						close(_sock_vec[i].get_socket_fd());
						_sock_vec.erase(_sock_vec.begin() + i);
						continue;
					}
					// std::cout << _sock_vec[i].buffer << std::endl;
					if (_sock_vec[i]._request == NULL)
					{
						_sock_vec[i]._request = new Request(_servers);
						_sock_vec[i]._res = -1;
					}
					if (_sock_vec[i]._request->is_finish() == -1)
					{
						_sock_vec[i]._request->parse_buffer(_sock_vec[i].buffer, t);
					}
					if (_sock_vec[i]._request->is_finish() == 1)
					{
						_sock_vec[i]._res = 0;
						FD_SET(_sock_vec[i].get_socket_fd(), &_writefds);
						FD_CLR(_sock_vec[i].get_socket_fd(), &_readfds);
					}
				}
			}
			else if (FD_ISSET(_sock_vec[i].get_socket_fd(), &_writefdsCopy))
			{
				// check if we have two servers with same port so I check which server to send to response
				if (_servers._servers.size() != _index)
				{
					for (size_t j = 0; j < _servers._servers.size(); j++)
					{
						if ((_sock_vec[i]._request->get_host() == _servers._servers[j]._server_name) && ( std::stoi(_sock_vec[i]._request->get_port()) == _servers._servers[j]._server_port))
						{
							_sock_vec[i].set_server(_servers._servers[j]);
						}
					}
				}
				// create obj of response for each request
				if (_sock_vec[i]._response == NULL)
					_sock_vec[i]._response = new Response(_sock_vec[i].get_server(), *_sock_vec[i]._request, _sock_vec[i].get_socket_fd());
				if (_sock_vec[i]._file_lenght == 0)
				{
					std::string tmp = _sock_vec[i]._response->getResponse().second;
					ret = _sock_vec[i]._response->getResponse().first;
					if (tmp.find('/') == std::string::npos)
					{
						ret += tmp;
						_sock_vec[i]._file_sent = tmp.length();
						_sock_vec[i]._file_lenght = tmp.length();
						_sock_vec[i]._check_is_path = -1;
					}
					else
					{
						_sock_vec[i]._file_lenght = getFileLength(tmp);
						_sock_vec[i]._file_fd = open((tmp).c_str(), O_RDONLY);
						if (_sock_vec[i]._file_fd < 0)
						{
							close(_sock_vec[i]._file_fd);
							continue;
						}
					}
				}
				if (_sock_vec[i]._check_is_path != -1)
				{
					if ((_sock_vec[i]._read_size = read(_sock_vec[i]._file_fd, _sock_vec[i]._read_buffer, sizeof(_sock_vec[i]._read_buffer))) >= 0)
					{
						_sock_vec[i]._file_sent += _sock_vec[i]._read_size;
					}
					else
					{
						continue;
					}
					std::string tmpbuf(_sock_vec[i]._read_buffer, _sock_vec[i]._read_size);
					ret += tmpbuf;	
				}
				if (send(_sock_vec[i].get_socket_fd(), ret.c_str(), ret.length(), 0) < 0)
				{
					FD_CLR(_sock_vec[i].get_socket_fd(), &_writefds);
					close(_sock_vec[i].get_socket_fd());
					close(_sock_vec[i]._file_fd);
					delete _sock_vec[i]._response;
					delete _sock_vec[i]._request;
					_sock_vec[i]._response = NULL;
					_sock_vec[i]._request = NULL;
					_sock_vec.erase(_sock_vec.begin() + i);
				}
				else if (_sock_vec[i]._file_lenght == _sock_vec[i]._file_sent)
				{
					_sock_vec[i]._file_lenght = 0;
					_sock_vec[i]._file_sent = 0;
					_sock_vec[i]._check_is_path = 0;
					close(_sock_vec[i]._file_fd);
					FD_SET(_sock_vec[i].get_socket_fd(), &_readfds);
					FD_CLR(_sock_vec[i].get_socket_fd(), &_writefds);
					if (_sock_vec[i]._request->get_header("Connection") == "close")
					{
						FD_CLR(_sock_vec[i].get_socket_fd(), &_readfds);
						close(_sock_vec[i].get_socket_fd());
						delete _sock_vec[i]._request;
						delete _sock_vec[i]._response;
						_sock_vec[i]._response = NULL;
						_sock_vec[i]._request = NULL;
						_sock_vec.erase(_sock_vec.begin() + i);
						continue;
					}
					delete _sock_vec[i]._request;
					delete _sock_vec[i]._response;
					_sock_vec[i]._response = NULL;
					_sock_vec[i]._request = NULL;
					// std::cout << "========= Response sent ============" << std::endl;
				}
			}
		}
	}
}
