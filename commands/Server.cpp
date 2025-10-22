#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Commands.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <algorithm>



Server::Server(int port, const std::string& password) 
    : _port(port), _password(password), _serverSocket(-1), _serverName("irc.server") {
    setupServer();
}

Server::~Server() {
    std::cout << "Server shutting down, cleaning up resources..." << std::endl;
    
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->first != -1) {
            close(it->first);
        }
        delete it->second;
    }
    _clients.clear();
    
    for (std::map<std::
        string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
    
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        if (_pollFds[i].fd != -1 && _pollFds[i].fd != _serverSocket) {
            close(_pollFds[i].fd);
        }
    }
    _pollFds.clear();
    
    if (_serverSocket != -1) {
        close(_serverSocket);
        _serverSocket = -1;
    }
    std::cout << "Cleanup completed." << std::endl;
}


void Server::setupServer() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1) {
        throw std::runtime_error("Failed to create socket");
    }
    
    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Failed to set socket options");
    }
    
    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);
    
    if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Failed to bind socket");
    }
    
    if (listen(_serverSocket, SOMAXCONN) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Failed to listen on socket");
    }
    
    struct pollfd serverPollFd;
    serverPollFd.fd = _serverSocket;
    serverPollFd.events = POLLIN;
    serverPollFd.revents = 0;
    _pollFds.push_back(serverPollFd);
    
    std::cout << "Server listening on port " << _port << std::endl;
}

void Server::run() {
    while (true) {
        int pollCount = poll(_pollFds.data(), _pollFds.size(), -1);
        
        if (pollCount < 0) {
            throw std::runtime_error("Poll error");
        }
        
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                if (_pollFds[i].fd == _serverSocket) {
                    handleNewConnection();
                } else {
                    handleClientData(_pollFds[i].fd);
                }
            }
        }
    }
}

void Server::handleNewConnection() {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    
    int clientFd = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd < 0) {
        std::cerr << "Error accepting connection" << std::endl;
        return;
    }
    
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    
    struct pollfd clientPollFd;
    clientPollFd.fd = clientFd;
    clientPollFd.events = POLLIN;
    clientPollFd.revents = 0;
    _pollFds.push_back(clientPollFd);
    
    Client* client = new Client(clientFd);
    _clients[clientFd] = client;
    
    std::cout << "New client connected from " << inet_ntoa(clientAddr.sin_addr) 
              << ":" << ntohs(clientAddr.sin_port) << " (FD: " << clientFd << ")" << std::endl;
}

void Server::handleClientData(int clientFd) {
    char buffer[1024];
    ssize_t bytesRead;
    
    Client* client = _clients[clientFd];
    if (!client) {
        return;
    }
    
    bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "Client disconnected (FD: " << clientFd << ")" << std::endl;
        } else {
            std::cerr << "Error reading from client (FD: " << clientFd << ")" << std::endl;
        }
        removeClient(clientFd);
        return;
    }
    
    buffer[bytesRead] = '\0';
    client->appendToBuffer(buffer);
    
    while (client->hasCompleteCommand()) {
        std::string command = client->extractCommand();
        std::cout << "Received from " << client->getNickname() << " (FD: " << clientFd << "): " << command << std::endl;
        processCommand(client, command);
    }
}

void Server::processCommand(Client* client, const std::string& command) {
    Commands commands(this);
    commands.execute(client, command);
}

void Server::removeClient(int clientFd) {
    Client* client = _clients[clientFd];
    if (client) {
        std::vector<Channel*> clientChannels = getClientChannels(client);
        for (size_t i = 0; i < clientChannels.size(); ++i) {
            clientChannels[i]->removeClient(client);
        }
        
        delete client;
        _clients.erase(clientFd);
    }
    
    for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == clientFd) {
            _pollFds.erase(it);
            break;
        }
    }
    
    close(clientFd);
    std::cout << "Client cleanup completed (FD: " << clientFd << ")" << std::endl;
}

Client* Server::getClient(const std::string& nickname) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == nickname) {
            return it->second;
        }
    }
    return NULL;
}

Channel* Server::getChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

Channel* Server::createChannel(const std::string& name, Client* creator) {
    Channel* channel = new Channel(name);
    _channels[name] = channel;
    
    // Use the creator parameter to set them as the first operator
    if (creator) {
        channel->addClient(creator);
        channel->setOperator(creator, true);
        std::cout << "Channel " << name << " created by " << creator->getNickname() << std::endl;
    }
    
    return channel;
}

void Server::removeChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
    }
}

const std::string& Server::getServerName() const {
    return _serverName;
}

bool Server::validatePassword(const std::string& password) const {
    return password == _password;
}

void Server::sendToClient(int fd, const std::string& message) {
    send(fd, message.c_str(), message.length(), 0);
}

std::vector<Channel*> Server::getClientChannels(Client* client) {
    std::vector<Channel*> channels;
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->hasClient(client)) {
            channels.push_back(it->second);
        }
    }
    return channels;
}