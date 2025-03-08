/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 17:03:53 by maglagal          #+#    #+#             */
/*   Updated: 2025/03/08 16:46:23 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "../Config/Config.hpp"
#include "../Parser/Parser.hpp"

//redirect response just from 304 and above

// std::map<int, std::ifstream *> Response::files;
std::map<std::string, std::string> Response::ContentTypeHeader;

//constructor
Response::Response()
{
    cgiScript = 0;
    clientFd = -1;
    FileType = 0;
    redirectFlag = 0;
    std::memset(contentLengthHeader, '\0', sizeof(contentLengthHeader));
    initializeContentHeader();
    initializeStatusRes();
    file = NULL;
    errorPage = NULL;
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
Response::~Response() 
{
   clearResponse();
}

//getters
std::map<std::string, std::string>& Response::getHeadersRes() { return Headers; }
int                                 Response::getClientFd() { return clientFd; }
std::string                         Response::getQueryString() { return queryString; }
int                                 Response::getStatusCode() { return statusCode; }
std::string                         Response::getStatusMssg() { return statusMssg; }
std::string                         Response::getHeader( std::string key ) { return Headers[key]; };
std::ifstream*                      Response::getFile() { return file; }

//setters
void                                Response::setClientFd( int nFd ) { clientFd = nFd; }
void                                Response::setQueryString( std::string value ) { queryString = value; }
void                                Response::setStatusCode(int value) { statusCode = value; };
void                                Response::setStatusMssg( std::string value ) { statusMssg = value; };
void                                Response::setHeader( std::string key, std::string value ) { Headers[key] = value; };
void                                Response::setFile(std::ifstream *nFile) { file = nFile; }


//other
void Response::initializeStatusRes()
{
    locationMatch = NULL;
    errorPage = NULL;
    file = NULL;
    resStatus.insert(std::make_pair(200, "OK"));
    resStatus.insert(std::make_pair(201, "Created"));
    resStatus.insert(std::make_pair(204, "No Content"));
    resStatus.insert(std::make_pair(206, "Partial Content"));
    resStatus.insert(std::make_pair(301, "Moved Permanently"));
    resStatus.insert(std::make_pair(302, "Moved Temporarily"));
    resStatus.insert(std::make_pair(303, "See Other"));
    resStatus.insert(std::make_pair(304, "Not Modified"));
    resStatus.insert(std::make_pair(305, "Use Proxy"));
    resStatus.insert(std::make_pair(306, "(Unused)"));
    resStatus.insert(std::make_pair(306, "Temporary Redirect"));
    resStatus.insert(std::make_pair(400, "Bad Request"));
    resStatus.insert(std::make_pair(403, "Forbidden"));
    resStatus.insert(std::make_pair(404, "Not Found"));
    resStatus.insert(std::make_pair(405, "Method Not Allowed"));
    resStatus.insert(std::make_pair(409, "Conflict"));
    resStatus.insert(std::make_pair(411, "Length Required"));
    resStatus.insert(std::make_pair(413, "Content Too Large"));
    resStatus.insert(std::make_pair(414, "URI Too Long"));
    resStatus.insert(std::make_pair(415, "Unsupported Media Type"));
    resStatus.insert(std::make_pair(416, "Requested Range Not Satisfiable"));
    resStatus.insert(std::make_pair(444, "No Response"));
    resStatus.insert(std::make_pair(500, "Internal Server Error"));
    resStatus.insert(std::make_pair(501, "Not Implemented"));
    resStatus.insert(std::make_pair(502, "Bad Gateway"));
    resStatus.insert(std::make_pair(504, "Gateway Timeout"));
}

void Response::initializeContentHeader()
{
    ContentTypeHeader[".txt"] = "text/plain";
    ContentTypeHeader[".html"] = "text/html";
    ContentTypeHeader[".htm"] = "text/html";
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
    cgiScript = 0;
    Headers.clear();
    statusMssg.clear();
    Headers["Connection"] = "keep-alive";
    Headers["Content-Length"] = "0";
    Headers["Server"] = "Webserv";
    Headers["Date"] = "0";
    statusMssg = "HTTP/1.1 ";
    statusCode = 0;
    FileType = 0;
    redirectFlag = 0;
    locationMatch = NULL;
    body.clear();
    finalRes.clear();
    filePath.erase();
    if (file)
    {
        file->close();
        delete file;
    }
    file = NULL;
    if (errorPage)
    {
        errorPage->close();
        delete errorPage;
    }
    errorPage = NULL;
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

std::string Response::getDate(time_t *time)
{
    struct tm datetime = *gmtime(time);
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
    statusMssg += statusCodeStr + " " + resStatus[statusCode] + "\r\n";
    if (body.empty() && statusCode != 204)
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
    if (!redirectFlag)
        Headers["Content-Type"] = "text/html";
    if (statusCode == 204)
    {
        Headers.erase("Content-Type");
        Headers.erase("Content-Length");
        body.erase();
    }
    if (!redirectFlag && statusCode == 201)
        Headers["Location"] = config.getClients()[clientFd].getRequest().getFileName();
    if (!redirectFlag && statusCode >= 500)
        Headers["Connection"] = "close";
}

void Response::successResponse(Config& config)
{
    char contentLengthHeader[150];
    statusMssg += "200 OK\r\n";
    
    if (!body.empty())
    {
        std::sprintf(contentLengthHeader, "%ld", body.length());
        Headers["Content-Length"] = contentLengthHeader;
    }
    if (!lastModified.empty())
        Headers["Last-Modified"] = lastModified;
    if (!file && FileType)
    {
        Headers["Accept-Ranges"] = "bytes";
        file = new(std::nothrow) std::ifstream(filePath.c_str(), std::ios::binary);
        if (!file)
        {
            clearResponse();
            statusCode = 500;
            generateRes(config);
            return ;
        }
    }
    Headers["Date"] = getDate();
}

void    Response::redirectionResponse(Request req, Config& config)
{
    char buff[150];
    char portChar[150];
    int  port;
    std::string statusCodeStr;
    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode] + "\r\n";
    port = config.getClients()[clientFd].getServer().getPort(); 
    sprintf(portChar, "%d", port);
    std::string host = config.getClients()[clientFd].getServer().getHost() + ":" + portChar;
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
    if (!redirectFlag)
    {
        std::cout << "locationheader -> " << locationHeader << std::endl;
        if (statusCode == 301 && locationHeader.length() == 0)
            locationHeader = "http://" + host + req.getPath() + "/";
        else if (statusCode == 301 && locationHeader.length() > 0)
            locationHeader = "http://" + host + locationHeader + "/";
        std::cout << "location header -> " << locationHeader << std::endl;
        setHeader("Location", locationHeader);
    }
    else
    {
        std::map<int, std::string>::iterator redirectIt;
        if (locationMatch)
            redirectIt = locationMatch->getRedirect().begin();
        else
            redirectIt = config.getClients()[clientFd].getServer().getRedirect().begin();
        if ((statusCode >= 301 && statusCode <= 303)
            || statusCode == 307 || statusCode == 308)
        {
            locationHeader = "http://" + host + redirectIt->second;
            setHeader("Location", locationHeader);
        }
        else
            body = redirectIt->second;
    }
    std::cout << "redirection response!!"<< std::endl;
    std::cout << "status code -> " << statusCode << std::endl;
    
    char contentLengthHeader[150];
    std::sprintf(contentLengthHeader, "%ld", body.length());
    Headers["Content-Type"] = "text/html";
    Headers["Content-Length"] = contentLengthHeader;
    Headers["Date"] = getDate();
}

void Response::rangeResponseFail(Config& config, Request& req)
{
    clearResponse();
    statusCode = 416;
    Headers["Content-Range"] = "bytes */" + Headers["Content-Length"];
    fillBody(config, req);
}

void Response::makeContentRangeHeader(Request& req, std::vector<std::string>& rangeNumbers,
        std::string& rangeNumber, std::string& contentLengthHeader, size_t& rangeEndNbr)
{
    char        rangeEndChar[150];
    char        rangeStartChar[150];
    size_t      rangeStartNbr = 0;
    size_t      length = req.strToDecimal(contentLengthHeader);
    std::string rangeHeader;

    if (rangeNumbers[0].length() && rangeNumbers[1].length())
    {
        if (rangeEndNbr > length - 1)
        {
            rangeEndNbr = length - 1;
            sprintf(rangeEndChar, "%ld", rangeEndNbr);
            rangeHeader = rangeNumbers[0] + '-' + rangeEndChar;
        }
        else
            rangeHeader = rangeNumber;
    }
    else
    {
        if (!rangeNumbers[0].length())
            rangeStartNbr = length - rangeEndNbr;
        else
            rangeStartNbr = req.strToDecimal(rangeNumbers[0]);
        sprintf(rangeStartChar, "%ld", rangeStartNbr);
        sprintf(rangeEndChar, "%ld", length - 1);
        std::string rangeStartString = rangeStartChar;
        std::string rangeEndString = rangeEndChar;
        rangeHeader = rangeStartString + '-' + rangeEndString;
    }
    Headers["Content-Range"] = "bytes " + rangeHeader + '/' + contentLengthHeader;
}

void Response::rangeResponse(Config& config, Request& req)
{
    if (!file)
    {
        file = new(std::nothrow) std::ifstream(filePath.c_str(), std::ios::binary);
        if (!file)
        {
            clearResponse();
            statusCode = 500;
            generateRes(config);
            return ;
        }
    }
    std::string contentLengthHeader = Headers["Content-Length"];
    size_t length = req.strToDecimal(contentLengthHeader);
    std::string statusCodeStr;
    char rangeContentLength[150];
    char buff[150];

    sprintf(buff, "%d", statusCode);
    statusCodeStr = buff;
    statusMssg += statusCodeStr + " " + resStatus[statusCode] + "\r\n";
    std::string range = req.getHeaders()["range"];
    size_t i = range.find("=");
    if (i == std::string::npos)
        return ;
    range.replace(i, 1, " ");
    std::string rangeNumber = range.substr(i + 1, range.size() - i);

    std::vector<std::string>rangeNumbers = Request::split(rangeNumber, 0, '-');
    size_t rangeStartNbr = req.strToDecimal(rangeNumbers[0]);
    size_t rangeEndNbr = req.strToDecimal(rangeNumbers[1]);

    if (((rangeNumbers[0].length() && !Parser::isNumber(rangeNumbers[0])))
        || (rangeNumbers[1].length() && !Parser::isNumber(rangeNumbers[1])))
        return rangeResponseFail(config, req);

    if (rangeNumbers[0].length() && rangeNumbers[1].length())
    {
        makeContentRangeHeader(req, rangeNumbers, rangeNumber,
                contentLengthHeader, rangeEndNbr);
        if (rangeStartNbr > rangeEndNbr)
            return rangeResponseFail(config, req);
        sprintf(rangeContentLength, "%ld", (rangeEndNbr - rangeStartNbr) + 1);
        setHeader("Content-Length", rangeContentLength);
    }
    else
    {
        if (rangeEndNbr > length - 1)
            rangeEndNbr = length;
        if (!isdigit(rangeNumber[0]))
            rangeNumber.erase(0, 1);
        if (!isdigit(rangeNumber[rangeNumber.length() - 1]))
            rangeNumber.erase(rangeNumber.length() - 1, 1);

        makeContentRangeHeader(req, rangeNumbers, rangeNumber,
            contentLengthHeader, rangeEndNbr);
        if (!rangeNumbers[0].length())
        { 
            sprintf(rangeContentLength, "%ld", rangeEndNbr);
            rangeStartNbr = length - rangeEndNbr;
        }
        else if (!rangeNumbers[1].length())
            sprintf(rangeContentLength, "%ld", length - rangeStartNbr);
        if (rangeStartNbr >= length)
            return rangeResponseFail(config, req);
        setHeader("Content-Length", rangeContentLength);
    }
    file->seekg(rangeStartNbr);  //this is where we start sending the video content
    Headers["Date"] = getDate();
    Headers["Last-Modified"] = lastModified;
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

void Response::verifyDirectorySlash(std::string fileName, Request& req)
{
    if (fileName.rfind("/") != fileName.length() - 1)
    {
        if (req.getMethod() == "DELETE")
            statusCode = 409;
        else
            statusCode = 301;
    }
    else if (req.getMethod() == "DELETE")
        statusCode = 204;
}

void Response::handleCgiScript(Config& config, std::string& fileName)
{
    std::cout << "handle cgi script!!\n";
    struct stat st;
    std::string cgiScriptExtension;
    std::string serverRoot = config.getClients()[clientFd].getServer().getRoot();
    size_t index = 0;

    cgiScript = 1;
    
    
    std::string cgi_dir = serverRoot + config.getClients()[clientFd].getServer().getCgiDir();
    if (*cgi_dir.rbegin() == '/')
        cgi_dir.erase(cgi_dir.length() - 1, 1);
    std::cout << "cgi dir -> " <<cgi_dir<< std::endl;
    if (!stat(cgi_dir.c_str(), &st))
    {
        if (!(st.st_mode & S_IFDIR))
        {
            statusCode = 500;
            return ;
        }
    }
    std::vector<std::string>::iterator extensionIt = config.getClients()[clientFd].getServer().getCgiExt().begin();
    while(extensionIt != config.getClients()[clientFd].getServer().getCgiExt().end())
    {
        index = fileName.find(*extensionIt);
        if (index != std::string::npos) {
            cgiScriptExtension = *extensionIt;
            break;
        }
        extensionIt++;
    }
    if (!cgiScriptExtension.length())
    {
        statusCode = 500;
        return ;
    }
    config.getClients()[clientFd].getCGI().setExtensionFile(cgiScriptExtension);
    std::string scriptPath = fileName.substr(0, index + cgiScriptExtension.length());
    if (!stat(scriptPath.c_str(), &st))
        statusCode = 200;
    else if (errno == EACCES)
        statusCode = 403;
    else
        statusCode = 404;
    if (statusCode != 200)
        return ;
    config.getClients()[clientFd].getCGI().setscriptFilePath(scriptPath);
    std::string pathInfo = fileName.substr(index + cgiScriptExtension.length());
    config.getClients()[clientFd].getCGI().setPathInfo(pathInfo);
}

//remove last / bcz /assets/main.py/ is not found for me but its same as /assets/main.py
void Response::searchForFile(Config& config, Request& req)
{
    struct stat st;
    std::string fileName;
    std::string serverRoot = config.getClients()[clientFd].getServer().getRoot();
    std::string cgiDir = config.getClients()[clientFd].getServer().getCgiDir();

    if (fileName == "/")
        fileName = serverRoot;
    else if (serverRoot.length() > 0 && serverRoot.rfind("/") == serverRoot.length() - 1) 
        serverRoot.erase(serverRoot.length() - 1);
    fileName = serverRoot + req.getPath();

    //seperating filename from querystring
    checkForQueryString(fileName);

    req.setPath(req.urlDecode(req.getPath()));
    fileName = req.urlDecode(fileName); 
    if (*cgiDir.rbegin() != '/')
        cgiDir += '/';

    std::cout << "req -> " <<  fileName << std::endl;
    
    if (!strncmp(req.getPath().c_str(), cgiDir.c_str(), cgiDir.length()))
        return (handleCgiScript(config, fileName));
    if (!stat(fileName.c_str(), &st))
    {
        if (st.st_mode & S_IFDIR || (!(st.st_mode & S_IRUSR)))
        {
            statusCode = 403;
            verifyDirectorySlash(fileName, req);
            if (statusCode == 403)
                checkAutoIndex(config, req);
        }
        else if ((st.st_mode & S_IFREG) && (st.st_mode & S_IRUSR))
        {
            if (req.getHeaders().find("range") != req.getHeaders().end())
            {
                statusCode = 206;
                filePath = fileName;
                lastModified = getDate(&st.st_mtime);
                sprintf(contentLengthHeader, "%ld", st.st_size);
                setHeader("Content-Length", contentLengthHeader);
                checkForFileExtension(fileName);
            }
            else
            {
                if (req.getMethod() == "DELETE")
                    statusCode = 204;
                else
                    statusCode = 200;
                if (statusCode == 200 || statusCode == 206)
                    FileType = 1;
                filePath = fileName;
                lastModified = getDate(&st.st_mtime);
                sprintf(contentLengthHeader, "%ld", st.st_size);
                setHeader("Content-Length", contentLengthHeader);
                checkForFileExtension(fileName);
            }
        }
    }
    else
        statusCode = 404;
}

int Response::sendBodyBytes(int epoll_fd)
{
    int bytesR = 0;
    if (file)
    {
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
            epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = clientFd;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, clientFd, &ev) != 0) {
                std::cerr << "epoll_ctl error: " << strerror(errno) << std::endl;
                return (-1);
            }
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

void Response::handleDeleteRequest(Config& config, Request& req)
{
  std::string serverRoot = config.getClients()[clientFd].getServer().getRoot();
  std::string requestedPath;

  if (serverRoot.length() > 0 && serverRoot.rfind("/") != serverRoot.length() - 1)
      requestedPath = serverRoot + "/" + req.getPath();
  else
      requestedPath = serverRoot + req.getPath();
  char requestPath[200];
  std::strcpy(requestPath, requestedPath.c_str());
  if (statusCode == 204)
  {
      if (rmrf(requestPath) == -1)
      {
          clearResponse();
          statusCode = 500;
          return ;
      }      
  }
  Headers.erase("Content-Type");
  Headers.erase("Content-Length");
}

void Response::fillBody(Config& config, Request& req)
{
    checkErrorPages(config, req);
    if (!redirectFlag)
        returnResponse(config);
    if (!redirectFlag && req.getMethod() == "DELETE" && statusCode == 204)
        handleDeleteRequest(config, req);
    if (statusCode == 200)
        successResponse(config);
    else if (statusCode == 206)
        rangeResponse(config, req);
    else if (statusCode >= 300 && statusCode <= 399)
        redirectionResponse(req, config);
    else
        generateRes(config);
}

void Response::sendResponse(Config& config, Request& req, int fd)
{
    if (cgiScript && statusCode == 200)
    {
        
        int status = 0;
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
    if (statusCode >= 400)
        config.closeConnection(fd);
}
