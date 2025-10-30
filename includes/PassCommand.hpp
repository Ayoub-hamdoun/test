#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "BaseCommand.hpp"

class PassCommand : public BaseCommand {
public:
    PassCommand(Server* server);
    ~PassCommand();

    void execute(Client* client, const std::vector<std::string>& args);
};

#endif