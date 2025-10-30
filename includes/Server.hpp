#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>

class Client;
class Channel;

class Server {
private:
    int _port;
    std::string _password;
    int _serverSocket;
    std::string _serverName;
    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;
    std::vector<struct pollfd> _pollFds;

    void setupServer();
    void handleNewConnection();
    void handleClientData(int clientFd);
    void processCommand(Client* client, const std::string& command);
    std::string toString(int value); // ADD THIS LINE

public:
    Server(int port, const std::string& password);
    ~Server();

    void run();
    void removeClient(int clientFd);
    
    Client* getClient(const std::string& nickname);
    Channel* getChannel(const std::string& name);
    Channel* createChannel(const std::string& name, Client* creator = NULL);
    void removeChannel(const std::string& name);
    
    const std::string& getServerName() const;
    bool validatePassword(const std::string& password) const;
    
    void sendToClient(int fd, const std::string& message);
    std::vector<Channel*> getClientChannels(Client* client);
    void sendWelcome(Client* client);
};

#endif