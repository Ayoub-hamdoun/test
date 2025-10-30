#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "BaseCommand.hpp"

class KickCommand : public BaseCommand {
public:
    KickCommand(Server* server);
    ~KickCommand();

    void execute(Client* client, const std::vector<std::string>& args);
};

#endif