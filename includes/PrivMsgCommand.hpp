#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "BaseCommand.hpp"

class PrivMsgCommand : public BaseCommand {
public:
    PrivMsgCommand(Server* server);
    virtual ~PrivMsgCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif