/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:25:38 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/01 11:09:11 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

// Getters
std::vector<Server> Config::getServer() { return Servers; }

// Setters
void Config::addServer(Server new_server) { Servers.push_back(new_server); }

// Functions
int Config::init_sockets() {
    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0) {
        std::cerr << "Cannot create epoll instance!" << std::endl;
        return (1);
    }
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR;
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (it->init_server())
            return (1);
        ev.data.fd = it->getSocket();
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, it->getSocket(), &ev) != 0) {
            std::cerr << strerror(errno) << std::endl;
            close(epoll_fd);
            return (1);
        }
    }
    epoll_event events[MAX_EVENTS];
    while (1) {
        int fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (fds < 0) {
            std::cerr << "Cannot wait on sockets!" << std::endl;
            close(epoll_fd);
            return (1);
        }
        for (int i = 0; i < fds; i++) {
            if (events[i].events & EPOLLIN) {
                int fd = events[i].data.fd;
                if (is_server_fd(fd)) {
                    accept_connection(fd, epoll_fd, ev);
                }
                else {
                    handle_client(fd);
                }
            }
            else if (events[i].events & EPOLLERR) {
                std::cerr << "Socket error on fd: " << events[i].data.fd << std::endl;
                close(events[i].data.fd);
                // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
            }
            else if (events[i].events & EPOLLOUT)
                std::cout << "EPOLLOUT\n";
        }
    }
    return (0);
}

int Config::is_server_fd(int fd) {
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        // std::cout << fd<< " == " << it->getSocket() << "\n";
        if (fd == it->getSocket())
            return (1);
    }
    return (0);
}

Server Config::get_server(int fd) {
    for (std::vector<Server>::iterator it = Servers.begin(); it != Servers.end(); it++) {
        if (fd == it->getSocket())
            return (*it);
    }
    return (Servers[0]);
}

int Config::accept_connection(int fd, int epoll_fd, epoll_event& ev) {
    while (true) {
        int new_client = accept(fd, NULL, NULL);
        if (new_client < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else {
                std::cerr << "accept error: " << strerror(errno) << std::endl;
                break;
            }
        }
        // std::cout << "New client accepted with fd: " << new_client << std::endl;
        // Server s = get_server(fd);
        // std::cout << "New connection to " << s.getHost()
        //     << ":" << s.getPort() << std::endl;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = new_client;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &ev) != 0) {
            std::cerr << "epoll_ctl error: " << strerror(errno) << std::endl;
            close(new_client);
        }
    }
    return (0);
}

int Config::handle_client(int fd) {
    char buff[1024];
    std::string str;
    Request request;
    int read_body = 0;
    std::string res =     "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 147\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "<!DOCTYPE html>"
                    "<html><body><form method=\"post\" enctype=\"multipart/form-data\">"
                        "<input type=\"file\" name=\"file\">"
                        "<button>Upload</button>"
                    "</form></body></html>";
    while (1) {
        ssize_t received = recv(fd, buff, sizeof(buff) - 1, 0);
        std::cout << "received: " << received << std::endl;
        if (received < 0) {
            std::cerr << "Failed to read!" << std::endl;
            close(fd);
            return (1);
        }
        else if (received == 0) {
            std::cout << "Connection closed!" << std::endl;
            close(fd);
            break;
        }
        else {
            // buff[received] = '\0';
            str.append(buff, received);
            size_t stop_p = str.find("\r\n\r\n");
            if (!read_body && stop_p != std::string::npos) {
                if (request.parse(str)) {
                    std::cerr << "Invalid Request" << std::endl;
                }
                str = str.substr(stop_p + 4);
                // std::cout << "str -> |" << str<< "|\n";
                read_body = 1;
            }
            if (read_body && request.getMethod() == "POST"
                && request.getHeaders().find("content-length") != request.getHeaders().end()) {
                // std::cout << "inside\n";
                std::cout << "POST str -> |" << str<< "|\n";
                size_t s = atoi(request.getHeaders()["content-length"].c_str());
                if (str.size() >= s) {
                    request.setBody(str);
                    break;
                }
            }
            else if (!read_body) {
                std::cout << "continue\n";
                std::cout << "str -> |" << str<< "|\n";
                continue;
            }
            else {
                std::cout << "break\n";
                break;
            }
        }
    }
    // std::cout << "Received: " << str << std::endl;
    std::cout << "=============================================\n";
    if (request.getMethod() == "GET") {
        send(fd, res.c_str(), res.size(), 0);
    }
    // if (request.parse(str)) {
    //     std::cerr << "Invalid Request" << std::endl;
    // }
    return (0);
}