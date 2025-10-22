#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "BaseCommand.hpp"

class JoinCommand : public BaseCommand {
public:
    JoinCommand(Server* server);
    virtual ~JoinCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif