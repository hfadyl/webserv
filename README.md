# what is a socket?:
    A socket is a way to speak to other programs using standard Unis file descriptors. When Unix programs do any sort I/O, they do it by reading or writing ot a file descriptor. A network connection is also a file.

## How do you get this file descriptor for network communication?

You call the socket() function. It returns the socket descriptor, and you can communicate through it by using the send() and recv() function. Since a socket is a file descriptor, you can also use write() and read(). However, send() and recv() offer much greater control over your data transmission.

## Two types of sockets:
    There are more than two types of internet sockets, but two are the most important: stream sockets, and datagram sockets. 

    1- Stream sockets (SOCK_STREAM):
       $- Stream sockets are reliable two-way connected communication streams. Items you put into the socket will arrive in order at the opposite end. They will also be error-free.
        $- Stream socket use TCP to make sure the data arrive in order.

    2- Datagram sockets (SOCK_DGRAM):
        $- Datagram sockets are connectionless, because you don’t have to maintain an open connection like with stream sockets.

        $- Online multiplayer games, audio streaming, and video conferencing are examples of applications that use datagram sockets. UDP is used, because it’s fast. UDP is not keeping track if packages arrived or not.




# SRC: https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

- What is a server?
https://www.youtube.com/watch?v=9J1nJOivdyw&ab_channel=WebConcepts

- Networking with C : client and server
https://medium.com/@chongye225/networking-with-c-cf15426cc270

- Use and Interpretation of HTTP Version Numbers
https://datatracker.ietf.org/doc/html/rfc2145

- How does the Internet work?
https://developer.mozilla.org/en-US/docs/Learn/Common_questions/How_does_the_Internet_work

- Writing an HTTP server from scratch
https://bhch.github.io/posts/2017/11/writing-an-http-server-from-scratch/

- How to write a HTTP server?
https://softwareengineering.stackexchange.com/questions/200821/how-to-write-a-http-server

- what you need to know to build a simple http serve
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

- Hyper Text Transfer Protocol
https://www.youtube.com/watch?v=0OrmKCB0UrQ

- books:
https://www.rfc-editor.org/search/rfc_search_detail.php?title=HTTP&page=All

- Programming UNIX Sockets in C
http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/

- Sockets - Server & Client c++
https://www.bogotobogo.com/cplusplus/sockets_server_client.php

- webserv Wiki
https://github.com/qingqingqingli/webserv/wiki

- How nginx processes a request
https://nginx.org/en/docs/http/request_processing.html

-  Dealing With and Getting Around Blocking Sockets 
http://dwise1.net/pgm/sockets/blocking.html

- Understanding Nginx Server and Location Block Selection Algorithms
https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms

- Beej's Guide to Network Programming
https://beej.us/guide/bgnet/html

- C++ Web Server from Scratch | Pt. 3: Listening Socket
https://www.youtube.com/watch?v=bEsRapsPAWI&ab_channel=EricOMeehan

- Structures for handling internet addresses
https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html

- HTTP Crash Course & Exploration
https://www.youtube.com/watch?v=iYM2zFP3Zn0&ab_channel=TraversyMedia
hh
