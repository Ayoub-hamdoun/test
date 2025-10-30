#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "BaseCommand.hpp"

class UserCommand : public BaseCommand {
public:
    UserCommand(Server* server);
    ~UserCommand();

    void execute(Client* client, const std::vector<std::string>& args);
};

#endif