#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include "BaseCommand.hpp"

class PartCommand : public BaseCommand {
public:
    PartCommand(Server* server);
    virtual ~PartCommand();
    virtual void execute(Client* client, const std::vector<std::string> &args);
};

#endif