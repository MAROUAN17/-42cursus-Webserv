/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/22 11:17:32 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/24 15:13:48 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include "Server.hpp"

// Constructor
Location::Location() {
    URI = "/";
    root = "/";
    index = "index.html";
    autoindex = true;
    allowed_methods.push_back("POST");
    allowed_methods.push_back("GET");
    allowed_methods.push_back("DELETE");
}

Location::Location(Server& copy) {
    URI = "/";
    root = copy.getRoot();
    error_page = copy.getErrorPage();
    index = copy.getIndex();
    autoindex = copy.getAutoindex();
    allowed_methods.push_back("POST");
    allowed_methods.push_back("GET");
    allowed_methods.push_back("DELETE");
    redirect = copy.getRedirect();
}

// Getters
std::string Location::getURI() { return URI; }
std::string Location::getRoot() { return root; }
std::string Location::getUploadPath() { return uploadPath; }
std::map<int, std::string>& Location::getErrorPage() { return error_page; }
std::string Location::getIndex() { return index; }
bool Location::getAutoindex() { return autoindex; }
std::vector<std::string>& Location::getAllowedMethods() { return allowed_methods; }
std::map<int, std::string> Location::getRedirect() { return redirect; }
std::vector<std::string>& Location::getCgiPath() { return cgi_path; }
std::vector<std::string>& Location::getCgiExt() { return cgi_ext; }

// Setters
void Location::setURI(std::string& n_URI) { URI = n_URI; }
void Location::setRoot(std::string& n_root) { root = n_root; }
void Location::setUploadPath(std::string& n_path) { uploadPath = n_path; }
void Location::setErrorPage(std::map<int, std::string>& n_ep) { error_page = n_ep; }
void Location::addErrorPage(int code, std::string path) {
    if (error_page.find(code) == error_page.end())
        error_page[code] = path;
}
void Location::setIndex(std::string& str) { index = str; }
void Location::setAutoindex(bool n_autoindex) { autoindex = n_autoindex; }
void Location::setAllowedMethods(std::vector<std::string>& methods) { allowed_methods = methods; }
void Location::setRedirect(int code, std::string page) { 
    if (redirect.size() == 0)
        redirect[code] = page;
}
void Location::addCgiPath(std::string path) { cgi_path.push_back(path); }
void Location::addCgiExt(std::string ext) { cgi_ext.push_back(ext); }