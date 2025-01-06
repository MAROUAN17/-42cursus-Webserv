/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/03 11:07:50 by oait-laa          #+#    #+#             */
/*   Updated: 2025/01/06 16:59:18 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UploadFile.hpp"

// Constructor
UploadFile::UploadFile() {
    fd = NULL;
    state = false;
}

// Getters
std::ofstream* UploadFile::getFd() { return (fd); }
std::string UploadFile::getFilename() { return (filename); }
std::string UploadFile::getBoundary() { return (boundary); }
bool UploadFile::getState() { return (state); }

// Setters
// void UploadFile::setFd(std::ofstream f) { fd = f; }
void UploadFile::setFilename(std::string name) { filename = name; }
void UploadFile::setState(bool s) { state = s; }
void UploadFile::setBoundary(std::string name) { boundary = name; }

// Functions
void UploadFile::newFilename() {
    struct timeval tv;
    std::ostringstream time;
    gettimeofday(&tv, NULL);
    time << (tv.tv_sec * 1000000) + tv.tv_usec;
    size_t pointPos = filename.rfind('.');
    if (pointPos != std::string::npos)
        filename.insert(pointPos, "_" + time.str());
    else
        filename.insert(filename.size(), "_" + time.str());
}

int UploadFile::openFile() {
    newFilename();
    fd = new std::ofstream(("../../goinfre/" + filename).c_str());
    if (fd->is_open()) {
        // std::cout << "file created for " << filename << std::endl;
        return (1);
    }
    else {
        std::cerr << strerror(errno) << std::endl;
        delete fd;
    }
    return (0);
}

// Destructor
UploadFile::~UploadFile() {
    // std::cout << "Destroyed file -> |" << filename << "|\n";
    if (fd) {
        fd->close();
        delete fd;
    }
}