/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maglagal <maglagal@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:09:59 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/05 11:38:57 by maglagal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"

class Server {
    private:
        int socket_fd;
        std::string server_name;
        std::string host;
        int port;
        std::string root;
        std::map<std::vector<int>, std::string> error_page;
        long long client_max_body_size;
        std::string index;
        std::string redirect;
        std::vector<Location> locations;
    public:
        // Contructor
        Server();

        // Getters
        int getSocket();
        std::string getServerName();
        std::string getHost();
        int getPort();
        std::string getRoot();
        std::map<std::vector<int>, std::string>& getErrorPage();
        long long getClientMaxBodySize();
        std::string getIndex();
        std::string getRedirect();
        std::vector<Location> getLocations();

        // Setters
        void setSocket(int s);
        void setServerName(std::string& name);
        void setHost(std::string& new_host);
        void setPort(int n_port);
        void setRoot(std::string& n_root);
        void setErrorPage(std::map<std::vector<int>, std::string>& n_ep);
        void setClientMaxBodySize(long long size);
        void setIndex(std::string& str);
        void setRedirect(std::string& page);
        void addLocation(Location& new_location);

        // Functions
        int initServer(std::vector<Server>& Servers, std::vector<Server>::iterator it);
        int checkPortDup(std::vector<Server>& Servers, std::vector<Server>::iterator it);

        // Destructor
        ~Server();
};

#endif