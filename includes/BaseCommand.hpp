#ifndef BASECOMMAND_HPP
#define BASECOMMAND_HPP

#include <string>
#include <vector>

class Server;
class Client;

class BaseCommand {
protected:
    Server* _server;
    
    void sendError(Client* client, int code, const std::string& message);
    void sendReply(Client* client, const std::string& reply);
    std::string toString(int value);
    
public:
    BaseCommand(Server* server);
    virtual ~BaseCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args) = 0;
};

#endif