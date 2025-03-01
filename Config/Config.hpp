/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oait-laa <oait-laa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/21 15:07:06 by oait-laa          #+#    #+#             */
/*   Updated: 2025/02/26 10:30:30 by oait-laa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include <sys/stat.h>
#include <arpa/inet.h>

#include "../Client/Client.hpp"
// #include "../Request/Request.hpp"
#define MAX_EVENTS 10

class Request;
class Response;

class Config {
    private:
        int                   epoll_fd;
        std::vector<Server>   Servers;
        std::map<int, Client> Clients;
        char                  **envP;
    public:
        // Getters
        std::vector<Server>    getServers();
        std::map<int, Client>& getClients();
        char**                 getEnvp();

        // Setters
        void addServer(Server new_server);
        void setEnvp( char **nEnvp );

        // Functions
        void    checkCgiScriptExecution(int fd);
        void    checkScriptTimeOut(int fd);
        int     startServers(char **envp);
        int     monitorServers(epoll_event& ev);
        int     isServerFd(int fd);
        int     acceptConnection(int fd, epoll_event& ev);
        int     handleClient(int fd);
        Server  getServer(int fd);
        static  long long timeNow();
        int     monitorTimeout();
        void    closeConnection(int fd);
        void    printLog(int fd);
};

#endif
