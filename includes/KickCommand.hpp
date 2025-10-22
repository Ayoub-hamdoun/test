#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "BaseCommand.hpp"
#include <vector>
#include <string>

class Client;

class KickCommand : public BaseCommand {
public:
    KickCommand(Server* server);
    virtual ~KickCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif