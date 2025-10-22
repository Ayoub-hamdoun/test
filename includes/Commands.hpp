#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>

class Server;
class Client;

class Commands {
private:
    Server* _server;
    
public:
    Commands(Server* server);
    ~Commands();
    
    void execute(Client* client, const std::string& commandLine);
};

#endif