/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/01/16 14:19:03 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>

//constructor
Response::Response() {
    Headers["Content-Type"] = "text/html";
    Headers["Connection"] = "keep-alive";
    Headers["Server"] = "Webserv";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
}

//getters
int         Response::getStatusCode() { return statusCode; };
std::string Response::getStatusMssg() { return statusMssg; };
std::string Response::getHeader( std::string key ) { return (Headers.find(key)->second); };

//setters
void Response::setStatusCode(int value) { statusCode = value; };
void Response::setStatusMssg( std::string value ) { statusMssg = value; };
void Response::setHeader( std::string key, std::string value ) { Headers.find(key)->second = value; };

//other
void Response::searchForFile(std::string fileName) {
    struct stat st;

    if (fileName != "/")
        fileName.erase(0, 1);
    else {
        statusCode = 200;
        return;
    }
    if (!stat(fileName.c_str(), &st)) {
        if (st.st_mode & S_IFDIR || (!(st.st_mode & S_IRUSR))) {
            statusCode = 403;
            return ;
        }
        else if ((st.st_mode & S_IFREG) && (st.st_mode & S_IRUSR)) {
            statusCode = 200;
            return ;
        }
    }
    statusCode = 404;
}

void Response::fillBody() {
    if (statusCode == 200) {
        statusMssg += "200 OK\r\n";
        body = "<!DOCTYPE html>"
             "<html><head></head><body><form method=\"post\" enctype=\"multipart/form-data\">"
             "<input type=\"file\" name=\"file\">"
             "<button>Upload</button>"
            "</form></body></html>";
    }
    else if (statusCode == 404) {
        statusMssg += "404 Not Found\r\n";
        body = "<!DOCTYPE html>"
              "<html><head>"
              "<style>"
              "h1, p {text-align:center}</style></head><body>"
              "<h1>404 Not Found</h1>"
              "<hr></hr>"
              "<p>Webserv</p>"
              "</body></html>";
    }
}

void Response::sendResponse(int fd) {
    std::map<std::string, std::string>::iterator it = Headers.begin();
    fillBody();
    if (statusCode == 403)
        statusMssg += "403 Forbidden\r\n";
    finalRes += statusMssg;
    while (it != Headers.end()) {
        std::string header = it->first + ": " + it->second;
        finalRes += header + "\r\n";
        it++;
    }
    finalRes += "\r\n";
    if (!body.empty())
        finalRes += body;
    send(fd, finalRes.c_str(), finalRes.size(), 0);
}

