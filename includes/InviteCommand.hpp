#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "BaseCommand.hpp"

class InviteCommand : public BaseCommand {
public:
    InviteCommand(Server* server);
    virtual ~InviteCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif