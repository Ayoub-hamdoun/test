#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "BaseCommand.hpp"

class UserCommand : public BaseCommand {
private:
    void sendWelcome(Client* client);
    
public:
    UserCommand(Server* server);
    virtual ~UserCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif