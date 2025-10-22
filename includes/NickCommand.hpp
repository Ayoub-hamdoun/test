#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include "BaseCommand.hpp"
#include <vector>
#include <string>

class Client;

class NickCommand : public BaseCommand {
private:
    void sendWelcome(Client* client);
    bool isValidNickname(const std::string& nick);
    
public:
    NickCommand(Server* server);
    virtual ~NickCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif