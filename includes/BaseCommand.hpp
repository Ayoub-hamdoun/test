#ifndef BASECOMMAND_HPP
#define BASECOMMAND_HPP

#include <string>
#include <vector>

class Server;
class Client;

class BaseCommand {
protected:
    Server* _server;

public:
    BaseCommand(Server* server);
    virtual ~BaseCommand();
    
    virtual void execute(Client* client, const std::vector<std::string>& args) = 0;
    
protected:
    void sendError(Client* client, const std::string& code, const std::string& message);
    void sendReply(Client* client, const std::string& code, const std::string& message);
    void sendRaw(Client* client, const std::string& message);
    std::string toString(int value);
};

#endif