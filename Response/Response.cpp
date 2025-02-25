/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/02/23 09:22:35 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../Config/Config.hpp"
#include "../Parser/Parser.hpp"


// std::map<int, std::ifstream *> Response::files;
std::map<std::string, std::string> Response::ContentTypeHeader;

//constructor
Response::Response() {
    file = NULL;
    initializeContentHeader();
    initializeStatusRes();
    Headers["Content-Type"] = "text/html";
    Headers["Connection"] = "keep-alive";
    Headers["Server"] = "Webserv";
    Headers["Content-Length"] = "0";
    Headers["Date"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
    char curr_dirChar[120];
    getcwd(curr_dirChar, 120);
    currentDirAbsolutePath = curr_dirChar;
}

//Destructor
Response::~Response() {
    if (file) {
        file->close();
        delete file;
    }
}

//getters
std::map<std::string, std::string>& Response::getHeadersRes() { return Headers; }
int                                 Response::getClientFd() { return clientFd; }
std::string                         Response::getQueryString() { return queryString; }
int                                 Response::getStatusCode() { return statusCode; };
std::string                         Response::getStatusMssg() { return statusMssg; };
std::string                         Response::getHeader( std::string key ) { return Headers[key]; };

//setters
void Response::setClientFd( int nFd ) { clientFd = nFd; }
void Response::setQueryString( std::string value ) { queryString = value; }
void Response::setStatusCode(int value) { statusCode = value; };
void Response::setStatusMssg( std::string value ) { statusMssg = value; };
void Response::setHeader( std::string key, std::string value ) { Headers[key] = value; };

//other
void Response::initializeStatusRes()
{
    locationMatch = NULL;
    resStatus.insert(std::make_pair(200, "OK\r\n"));
    resStatus.insert(std::make_pair(201, "Created\r\n"));
    resStatus.insert(std::make_pair(206, "Partial Content\r\n"));
    resStatus.insert(std::make_pair(301, "Moved Permanently\r\n"));
    resStatus.insert(std::make_pair(302, "Moved Temporarily\r\n"));
    resStatus.insert(std::make_pair(400, "Bad Response\r\n"));
    resStatus.insert(std::make_pair(403, "Forbidden\r\n"));
    resStatus.insert(std::make_pair(404, "Not Found\r\n"));
    resStatus.insert(std::make_pair(405, "Method Not Allowed\r\n"));
    resStatus.insert(std::make_pair(411, "Length Required\r\n"));
    resStatus.insert(std::make_pair(413, "Content Too Large\r\n"));
    resStatus.insert(std::make_pair(414, "URI Too Long\r\n"));
    resStatus.insert(std::make_pair(500, "Internal Server Error\r\n"));
    resStatus.insert(std::make_pair(501, "Not Implemented\r\n"));
    resStatus.insert(std::make_pair(502, "Bad Gateway\r\n"));
    resStatus.insert(std::make_pair(504, "Gateway Timeout\r\n"));
}

void Response::initializeContentHeader()
{
    ContentTypeHeader[".txt"] = "text/plain";
    ContentTypeHeader[".html"] = "text/html";
    ContentTypeHeader[".css"] = "text/css";
    ContentTypeHeader[".js"] = "application/javascript";
    ContentTypeHeader[".pdf"] = "application/pdf";
    ContentTypeHeader[".png"] = "image/png";
    ContentTypeHeader[".jpg"] = "image/jpeg";
    ContentTypeHeader[".jpeg"] = "image/jpeg";
    ContentTypeHeader[".mp4"] = "video/mp4";
    ContentTypeHeader[".mp3"] = "audio/mpeg";
    ContentTypeHeader[".json"] = "application/json";
}


void    Response::addHeadersToResponse()
{
    std::map<std::string, std::string>::iterator it = Headers.begin();
    while (it != Headers.end()) {
        std::string header = it->first + ": " + it->second;
        finalRes += header + "\r\n";
        it++;
    }
}

void    Response::clearResponse()
{
    Headers.clear();
    statusMssg.clear();
    Headers["Connection"] = "keep-alive";
    Headers["Content-Length"] = "0";
    Headers["Server"] = "Webserv";
    Headers["Date"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
    locationMatch = NULL;
    body.clear();
    finalRes.clear();
    if (file)
        delete file;
    file = NULL;
}

std::string Response::getDate()
{
    time_t timestamp = time(NULL);
    struct tm datetime = *gmtime(&timestamp);
    char now[50];

    strftime(now, 50, "%a, %d %b %Y %H:%M:%S GMT", &datetime);
    std::string res = now;
    return (res);
}

void Response::generateRes(Config& config)
{
    char buff[150];
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode];
    if (body.empty())
    {
        body = "<!DOCTYPE html>"
                "<html>"
                "<head><title>" + statusCodeStr + " " + resStatus[statusCode] + "</title></head>"
                "<body>"
                "<center><h1>" + statusCodeStr + " " + resStatus[statusCode] + "</h1></center>"
                "<hr><center>Webserv</center>"
                "</body>"
                "</html>";
    }
    char contentLength[150];
    std::sprintf(contentLength, "%ld", body.length());
    Headers["Content-Length"] = contentLength;
    Headers["Date"] = getDate();
    Headers["Content-Type"] = "text/html";
    if (statusCode == 201)
        Headers["Location"] = config.getClients()[clientFd].getRequest().getFileName();
    if (statusCode >= 500)
        Headers["Connection"] = "close";
}

void Response::successResponse(Request req)
{
    char contentLengthHeader[150];
    statusMssg += "200 OK\r\n";
    if (req.getPath() == "/") {
        body = "<!DOCTYPE html>"
            "<html><head></head><body><form method=\"post\" enctype=\"multipart/form-data\">"
            "<input type=\"file\" name=\"file\">"
            "<button>Upload</button>"
            "</form></body></html>";
        std::sprintf(contentLengthHeader, "%ld", body.length());
        Headers["Content-Length"] = contentLengthHeader;
        Headers["Date"] = getDate();
    }
    else {
        if (!body.empty()) {
            std::sprintf(contentLengthHeader, "%ld", body.length());
            Headers["Content-Length"] = contentLengthHeader;
        }
        if (!file)
            file = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
        Headers["Accept-Ranges"] = "bytes";
        Headers["Date"] = getDate();
    }
}

void    Response::redirectionResponse(Request req, Config& config)
{
    char buff[150];
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode];
    if (body.empty())
    {
        body = "<!DOCTYPE html>"
                "<html>"
                "<head><title>" + statusCodeStr + " " + resStatus[statusCode] + "</title></head>"
                "<body>"
                "<center><h1>" + statusCodeStr + " " + resStatus[statusCode] + "</h1></center>"
                "<hr><center>Webserv</center>"
                "</body>"
                "</html>";
    }
    if (statusCode == 301) 
    {
        int port = config.getClients()[clientFd].getServer().getPort(); 
        char portChar[150];
        sprintf(portChar, "%d", port);
        std::string host = config.getClients()[clientFd].getServer().getHost() + ":" + portChar;
        locationHeader = "http://" + host + req.getPath() + "/";
    }
    setHeader("Location", locationHeader);
    char contentLengthHeader[150];
    std::sprintf(contentLengthHeader, "%ld", body.length());
    Headers["Content-Length"] = contentLengthHeader;
    Headers["Date"] = getDate();
}

void Response::rangeResponse(Request req) {
    if (!file)
        file = new std::ifstream(req.getPath().erase(0, 1).c_str(), std::ios::binary);
    char buff[150];
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode];
    std::string range = req.getHeaders()["range"];
    size_t i = range.find("=");
    if (i == std::string::npos)
        return ;
    range.replace(i, 1, " ");
    if (getHeader("Content-Type") == "video/mp4"
            || getHeader("Content-Type") == "audio/mpeg") {
        char buff2[150];
        size_t length = req.strToDecimal(Headers["Content-Length"]);
        sprintf(buff2, "%ld", length - 1);
        Headers["Content-Range"] = range + buff2 + '/' + Headers["Content-Length"]; // construct the header content-range with the corresponding values
        std::string rangeNumber = range.substr(i, range.size() - i);
        long long rangeStart = req.strToDecimal(rangeNumber);
        file->seekg(rangeStart);  //this is where we start sending the video content
        size_t len = req.strToDecimal(Headers["Content-Length"]);
        char rangeContentLength[150];
        sprintf(rangeContentLength, "%lld", len - rangeStart);
        setHeader("Content-Length", rangeContentLength);
        Headers["Date"] = getDate();
    }
}

void Response::checkForFileExtension(std::string extension)
{
    size_t pos = extension.rfind(".");
    if (pos != std::string::npos) {
        extension.erase(0, pos);
        std::map<std::string, std::string>::iterator it = ContentTypeHeader.begin();
        while(it != ContentTypeHeader.end())
        {
            if (it->first == extension) {
                setHeader("Content-Type", it->second);
                return ;
            }
            it++;
        }
    }
    setHeader("Content-Type", "application/stream-octet");
}

void Response::checkForQueryString(std::string& fileName)
{
    size_t index = fileName.find("?");
    if (index != std::string::npos)
    {
        queryString = fileName.substr(index + 1);
        fileName.erase(index);
    }
}

void Response::vertifyDirectorySlash(std::string fileName)
{
    size_t i = fileName.rfind("/");
    if (i != fileName.length() - 1)
        statusCode = 301;
}

void Response::searchForFile(Request& req)
{
    struct stat st;
    std::string fileName = req.getPath();
    char buff3[150];

    //seperating filename from querystring
    checkForQueryString(fileName);
    req.setPath(req.urlDecode(req.getPath()));
    fileName = req.urlDecode(fileName);
    if (fileName != "/")
        fileName.erase(0, 1);
    else {
        statusCode = 200;
        setHeader("Content-Type", "text/html");
        return ;
    }
    if (!stat(fileName.c_str(), &st)) {
        if (st.st_mode & S_IFDIR || (!(st.st_mode & S_IRUSR))) {
            statusCode = 403;
            vertifyDirectorySlash(fileName);
            return ;
        }
        else if ((st.st_mode & S_IFREG) && (st.st_mode & S_IRUSR)) {
            if (req.getHeaders().find("range") != req.getHeaders().end()) {
                statusCode = 206;
                sprintf(buff3, "%ld", st.st_size);
                setHeader("Content-Length", buff3);
                checkForFileExtension(fileName);
                return ;
            }
            statusCode = 200;
            sprintf(buff3, "%ld", st.st_size);
            setHeader("Content-Length", buff3);
            if (st.st_mode & S_IFDIR)
                vertifyDirectorySlash(fileName);
            checkForFileExtension(fileName);
            return ;
        }
    }
    statusCode = 404;
}

int Response::sendBodyBytes()
{
    int bytesR = 0;
    if (file) {
        char buff[1024];
        // if marouan updates, update timeout of client here ---------
        file->read(buff, 1024);
        if (file->eof()) 
        {
            bytesR = file->gcount();
            if (send(clientFd, buff, bytesR, 0) == -1)
            {
                std::cerr << "Error : Send Fail" << std::endl;
                return (-1);
            }
            file->close();
            delete file;
            file = NULL;
            return (0);
        }
        bytesR = file->gcount();
        if (send(clientFd, buff, bytesR, 0) == -1)
        {
            std::cerr << "Error : Send Fail" << std::endl;
            return (-1);
        }
    }
    return (0);
}

void Response::fillBody(Config& config, Request req)
{
    if (statusCode != 301)
        checkAutoIndexAndErrorPages(config, req);

    //when matching a location should not enter here!!
    if (statusCode == 200)
        successResponse(req);
    else if (statusCode == 206)
        rangeResponse(req);
    else if (statusCode == 301 || statusCode == 302)
        redirectionResponse(req, config);
    else
        generateRes(config);
}

void Response::sendResponse(Config& config, Request& req, int fd)
{
    clientFd = fd;

    if (req.getPath().find("/cgi-bin/") != std::string::npos && statusCode == 200) {
        int status;
        status = config.getClients()[fd].getCGI().execute_cgi_script(config, *this, clientFd, req);
        if (fd != 0)
        {
            config.checkCgiScriptExecution(fd);
            config.checkScriptTimeOut(fd);
        }
        if (!config.getClients()[fd].getCGI().getChildStatus() && !status)
            return ;
    }
    fillBody(config, req);
    finalRes += statusMssg;

    //add headers to final response
    addHeadersToResponse();

    // add body to final response
    finalRes += "\r\n";
    if (!body.empty())
        finalRes += body;
    send(clientFd, finalRes.c_str(), finalRes.length(), 0);
    if (statusCode >= 500)
        config.closeConnection(fd);
}
