/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 10:25:22 by oait-laa          #+#    #+#             */
/*   Updated: 2025/03/05 17:21:50 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Constructor
Server::Server() {
    socket_fd = -1;
    server_name.push_back("Server");
    host = "localhost";
    port = 8080;
    root = "/";
    autoindex = false;
    // std::vector<int> err;
    // err.push_back(404);
    // error_page.insert(std::pair<std::vector<int>, std::string>(err, "/Error_pages/404.html"));
    client_max_body_size = 0;
    index = "index.html";
}

// Getters
int Server::getSocket() { return socket_fd; }
std::vector<std::string>& Server::getServerName() { return server_name; }
std::string Server::whichServerName(std::string str) {
    for(std::vector<std::string>::iterator it = server_name.begin(); it != server_name.end(); it++) {
        if (str == *it)
            return (*it);
    }
    return host;
}
std::string Server::getHost() { return host; }
int Server::getPort() { return port; }
std::string Server::getRoot() { return root; }
bool Server::getAutoindex() { return autoindex; }
std::map<int, std::string>& Server::getErrorPage() { return error_page; }
long long Server::getClientMaxBodySize() { return client_max_body_size; }
std::string Server::getIndex() { return index; }
std::map<int, std::string>& Server::getRedirect() { return redirect; }
std::vector<Location>& Server::getLocations() { return locations; }

// Setters
void Server::setSocket(int s) { socket_fd = s; }
void Server::setServerName(std::string& name) { server_name.push_back(name); }
void Server::setHost(std::string& new_host) { host = new_host; }
void Server::setPort(int n_port) { port = n_port; }
void Server::setRoot(std::string& n_root) { root = n_root; }
void Server::setAutoindex(bool n_autoindex) { autoindex = n_autoindex; }
void Server::setErrorPage(std::map<int, std::string>& n_ep) { error_page = n_ep; }
void Server::addErrorPage(int code, std::string path) {
    if (error_page.find(code) == error_page.end())
        error_page[code] = path;
}
void Server::setClientMaxBodySize(long long size) { client_max_body_size = size; }
void Server::setIndex(std::string& str) { index = str; }
void Server::setRedirect(int code, std::string page) {
    if (redirect.size() == 0)
        redirect[code] = page;
}
void Server::addLocation(Location& new_location) { locations.push_back(new_location); }

// Functions
int Server::checkPortDup(std::vector<Server>& Servers, std::vector<Server>::iterator it) {
    for (std::vector<Server>::iterator it2 = Servers.begin(); it2 != it; it2++) {
        if (it2->getPort() == it->getPort() && it2->getHost() == it->getHost()) {
            return (0);
        }
    }
    return (1);
}

int Server::checkNameDup(std::vector<Server>& Servers, std::vector<Server>::iterator it, std::string& s_name) {
    for (std::vector<Server>::iterator it2 = Servers.begin(); it2 != it; it2++) {
        if (it2->getPort() == it->getPort() && it2->getHost() == it->getHost()) {
            for (std::vector<std::string>::iterator s_it = it2->getServerName().begin(); s_it != it2->getServerName().end(); s_it++) {
                if (s_name == *s_it)
                    return (1);
            }
        }
    }
    return (0);
}

int Server::initServer(std::vector<Server>& Servers, std::vector<Server>::iterator it) {
    // struct sockaddr_in address;
    struct addrinfo hints, *res;
    char buff[6];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    std::sprintf(buff, "%d", port);
    int status = getaddrinfo(host.c_str(), buff, &hints, &res);
    if (status != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(status) << std::endl;
        return (1);
    }
    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_fd < 0) {
        freeaddrinfo(res);
        std::cerr << "Cannot create socket!" << std::endl;
        return (1);
    }
    int val = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        freeaddrinfo(res);
        std::cerr << "Setsockopt Error!" << std::endl;
        return (1);
    }
    if (bind(socket_fd, res->ai_addr, res->ai_addrlen) != 0 && checkPortDup(Servers, it)) {
        freeaddrinfo(res);
        std::cerr << strerror(errno) << std::endl;
        return (1);
    }
    freeaddrinfo(res);
    if (listen(socket_fd, 10) != 0) {
        std::cerr << "Cannot listen to socket!" << std::endl;
        return (1);
    }
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    for(std::vector<std::string>::iterator it2 = server_name.begin(); it2 != server_name.end(); it2++) {
        if (checkNameDup(Servers, it, *it2) == 0)
            std::cout << *it2 << ": listening to port " << port << "...\n";
    }
    return (0);
}

// Destructor
Server::~Server() {
    if (socket_fd > 0)
        close(socket_fd);
}