#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include "BaseCommand.hpp"
#include <vector>
#include <string>

class PrivMsgCommand : public BaseCommand {
private:
    std::vector<std::string> splitTargets(const std::string& targetsStr);
    bool sendToTarget(Client* client, const std::string& target, const std::string& message);
    bool sendToChannel(Client* client, const std::string& channelName, const std::string& message);
    bool sendToUser(Client* client, const std::string& nickname, const std::string& message);

public:
    PrivMsgCommand(Server* server);
    ~PrivMsgCommand();

    void execute(Client* client, const std::vector<std::string>& args);
};

#endif