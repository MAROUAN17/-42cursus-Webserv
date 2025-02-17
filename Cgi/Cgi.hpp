/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/30 18:22:52 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/17 12:05:26 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <errno.h>
#include <limits>
#include <filesystem>
#include <sys/wait.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "../Response/Response.hpp"

class Response;
class Request;

class CGI {
    private :
        std::map<std::string, std::string> executablePaths;
        std::vector<std::string>headersInScript;
        char *envs[8];
        char *argv[3];
        std::string scriptRelativePath;
        std::string scriptFileName;
        std::string extensionFile;
        std::string cgiRes;
        std::string ResBody;
        char*       executablePathArray;
        char*       absoluteFilePath;
    public :
        CGI();
        ~CGI();
        void execute_cgi_script(Response& res, int fd, Request req);
        void initializeVars(Response& res, Request req);
        void setEnvVars(Request req, Response& res);
        void findExecutablePath();
        void read_cgi_response(int fd_r);
        void sendServerResponse(int fd, Response& res, Request req);
        void findHeadersInsideScript(Request req, Response& res);
        void convertHeaderToCamelCase(std::string& value);
        void defineResponseStatusMssg(Response& res);
};

#endif