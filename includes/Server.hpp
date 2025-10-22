#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <stdexcept>

class Client;
class Channel;

class Server {
private:
    int _port;
    std::string _password;
    int _serverSocket;
    std::vector<struct pollfd> _pollFds;
    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;
    std::string _serverName;
    
    void setupServer();
    void handleNewConnection();
    void handleClientData(int clientFd);
    void processCommand(Client* client, const std::string& command);
    void removeClient(int clientFd);

public:
    Server(int port, const std::string& password);
    ~Server();
    
    void run();
   // void cleanup();
    bool validatePassword(const std::string& password) const;
    Client* getClient(const std::string& nickname);
    Channel* getChannel(const std::string& name);
    Channel* createChannel(const std::string& name, Client* creator);
    void removeChannel(const std::string& name);
    const std::string& getServerName() const;
    void sendToClient(int fd, const std::string& message);
    std::vector<Channel*> getClientChannels(Client* client);
};

#endif