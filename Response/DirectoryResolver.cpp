/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DirectoryResolver.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 09:49:11 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/23 11:39:36 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../Config/Config.hpp"
//error pages
//if an error is not matched you should use a default
//you should match the error pages in root

//location (list or not)
//1- match request url with location url
//2- concatenate path request with location root
//3- if index.html in the dir = not list
//4- if not index.html is there check autoindex on or off
//5- if not index.html and not autoindex = 404 not found

int    Response::comparingReqWithLocation(std::string locationPath, std::string reqPath) {
    // std::cout << locationPath << std::endl;
    // std::cout << reqPath << std::endl;
    size_t i = locationPath.find(reqPath);
    std::string rest = locationPath.substr(i);
    return (0);
}

void    Response::showIndexFile(std::string indexFilePath) {
    std::ifstream indexFile(indexFilePath.c_str());
    std::string buff;
    statusCode = 200;
    while (std::getline(indexFile, buff))
        body += buff;
}

std::string Response::urlEncode(std::string path) {
    std::string res;
    for (std::string::iterator i = path.begin(); i != path.end(); i++) {
        if (!isalnum(*i) && *i != '-' && *i != '_' && *i != '.' && *i != '~' && *i != '/') {
            int n = static_cast<int>(*i);
            std::stringstream s;
            s << '%' << std::setw(2) << std::setfill('0') << std::uppercase 
                << std::hex << n << std::nouppercase;
            res += s.str();
        }
        else 
            res += *i;
    }
    return (res);
}

void Response::listDirectories(std::string reqPath) {
    struct dirent *stDir;
    struct stat   st;
    std::string row;
    std::string dirAsbolute = currentDirAbsolutePath + reqPath;
    std::string direntName;
    std::string direntPath;
    std::string EncodedPath;

    statusCode = 200;
    DIR *dir = opendir(dirAsbolute.c_str());
    std::string lDirectoriesPage = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                    "<style>p {text-align:left;font-size: small} h1 {text-align: left; font-size: large}</style>"
                "</head>"
                "<body>"
                "<h1>Webserv</h1>"
                "<hr></hr>";
    
    while ((stDir = readdir(dir))) {
        direntName = stDir->d_name;
        if (direntName != ".") {
            direntPath = reqPath + direntName;
            EncodedPath = urlEncode(reqPath + direntName);
            // std::cout << "direntPath -> " << direntPath << std::endl;
            std::string absoluteDirentPath = currentDirAbsolutePath + direntPath;
            if (!stat(absoluteDirentPath.c_str(), &st) && st.st_mode & S_IFDIR) {
                EncodedPath += "/";
                direntName += "/";
            }
            row = "<a href=\"" + EncodedPath + "\">" + "<p>" + direntName;
            row += "</p></a>";
            lDirectoriesPage += row;
        }
    }

    lDirectoriesPage += "</body>";
    lDirectoriesPage += "</html>";
    
    body = lDirectoriesPage;
}

void Response::matchReqPathWithLocation(Config& config, Location loc, std::string reqPath, std::string toMatch) {
    struct stat st;
    struct stat reqPathCheck;
    std::string locationPath = loc.getURI();
    
    std::string indexFile = reqPath + loc.getIndex();
    std::string aIndexFile = currentDirAbsolutePath + indexFile;
    std::string reqPathAsbsolute = currentDirAbsolutePath + reqPath;

    if (!strncmp(toMatch.c_str(), reqPath.c_str(), toMatch.length()))
    {
        
        if (!stat(reqPathAsbsolute.c_str(), &reqPathCheck)
            && (reqPathCheck.st_mode & S_IFDIR) && loc.getAutoindex())
        {
            if (!stat(aIndexFile.c_str(), &st) && st.st_mode & S_IFREG)
                showIndexFile(aIndexFile);
            else {
                listDirectories(reqPath);
            }
        }
        else if (!stat(aIndexFile.c_str(), &st) && st.st_mode & S_IFREG)
            showIndexFile(aIndexFile);
        else if (checkDefinedErrorPage(config.getClients()[clientFd].getServer().getRoot(),
            loc.getErrorPage())) // should not enter just when not found
        {
            std::cerr << "error page location!!!" << std::endl;
            return ;
        }
    }
    checkDefinedErrorPage(config.getClients()[clientFd].getServer().getRoot(),
        config.getClients()[clientFd].getServer().getErrorPage());
}

void Response::checkAutoIndex(Config& config, Request req) {
    std::string root;
    std::string uri;
    std::string nUri;
    std::string pathMatch;

    std::vector<Location>::iterator itLocations = config.getClients()[clientFd].getServer().getLocations().begin();
    while (itLocations != config.getClients()[clientFd].getServer().getLocations().end()) {
        uri = (*itLocations).getURI();
        if (uri.rfind("/") == 0 && uri.length() != 1)
        {
            uri = uri + "/";
            (*itLocations).setURI(uri);
        }
        root = (*itLocations).getRoot();
        if (root != "/")
            pathMatch = root + uri;
        else
            pathMatch = uri;
        if (uri.length() > 0 && uri[0] == '/') //dir should have a leading slash 
            matchReqPathWithLocation(config, *itLocations, req.getPath(), pathMatch);
        itLocations++;
    }
}

int    Response::checkDefinedErrorPage(std::string rootPath, std::map<int, std::string> error_page) {
    std::map<int, std::string>::iterator it = error_page.begin();
    while (it != error_page.end()) {
        if (it->first == statusCode)
        {
            returnDefinedPage(rootPath, it->second);
            return (1);
        }
        it++;
    }
    return (0);
}

void    Response::returnDefinedPage(std::string rootPath, std::string errorPageFile) {
    std::string buffer;
    if (rootPath == "/")
        errorPageFile = currentDirAbsolutePath + errorPageFile;
    else
        errorPageFile = rootPath + errorPageFile;
    // std::cout << "Error page " << errorPageFile<< std::endl;
    std::ifstream definedPage(errorPageFile.c_str());
    if (!definedPage.is_open()) {
        std::cerr << "Failed to open defined file " << errorPageFile << std::endl;
        return ;
    }
    while (std::getline(definedPage, buffer))
        body += buffer;
}
