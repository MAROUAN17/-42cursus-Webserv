🧠 Overview

webserv is a lightweight, custom-built HTTP server inspired by NGINX.
The goal of this project is to understand and implement the core of the HTTP protocol and build a server capable of serving web content, handling requests, and managing multiple connections efficiently.

This project is part of the 42 Network curriculum, focusing on network programming, I/O multiplexing, and system-level C++ development.

⚙️ Features

🧱 Core Functionality

- Full implementation in C++98, with no external libraries

- Handles multiple clients simultaneously using poll(), select(), epoll(), or kqueue()

- Non-blocking I/O — the server never blocks on reads or writes

- Configurable via a custom configuration file (inspired by NGINX syntax)

- Supports multiple ports and interfaces on a single instance

🌍 HTTP Support

- Implements HTTP/1.0 (and partially HTTP/1.1) semantics

- Supports the following methods:
   - GET — retrieve resources
   - POST — send data or upload files
   - DELETE — remove resources

- Generates accurate HTTP response codes

- Handles client disconnections gracefully

- Provides default error pages

📁 File Management

- Serve fully static websites from a directory

- Directory listing (enable/disable per route)

- Custom index files (like index.html)

- File upload support for clients

- Maximum body size configurable per server block

⚙️ Configuration File

The configuration file defines:

- Ports and interfaces to listen on

- Root directories for routes

- Allowed HTTP methods

- Default files and error pages

- Upload directories and CGI handlers

- Optional redirections

- Directory listing toggle
