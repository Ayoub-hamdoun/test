#include "../includes/JoinCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>

JoinCommand::JoinCommand(Server* server) : BaseCommand(server) {}
JoinCommand::~JoinCommand() {}

void JoinCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, "451", ":You have not registered");
        return;
    }
    
    if (args.empty()) {
        sendError(client, "461", "JOIN :Not enough parameters");
        return;
    }
    
    std::string channelName = args[0];
    std::string channelKey = args.size() > 1 ? args[1] : "";
    
    if (channelName.empty() || channelName[0] != '#'  || channelName.length() > 50) {
        sendError(client, "403", channelName + " :No such channel");
        return;
    }
    
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        channel = _server->createChannel(channelName, client);
        std::cout << "Created new channel: " << channelName << std::endl;
    }
    
    if (channel->isInviteOnly() && !channel->isInvited(client)) {
        sendError(client, "473", channelName + " :Cannot join channel (+i)");
        return;
    }
    
    if (!channel->getKey().empty() && channel->getKey() != channelKey) {
        sendError(client, "475", channelName + " :Cannot join channel (+k)");
        return;
    }
    
    if (channel->getUserLimit() > 0 && channel->getClientCount() >= static_cast<size_t>(channel->getUserLimit())) {
        sendError(client, "471", channelName + " :Cannot join channel (+l)");
        return;
    }
    
    channel->addClient(client);
    if (channel->getClientCount() == 1) {
        channel->setOperator(client, true);
    }
    
    channel->removeInvited(client);
    
    std::string joinMsg = ":" + client->getUserMask() + " JOIN " + channelName + "\r\n";
    channel->broadcast(joinMsg);
    
    channel->sendTopic(client, _server->getServerName());
    channel->sendNamesList(client, _server->getServerName());
    
    std::cout << "Client " << client->getNickname() << " joined channel " << channelName << std::endl;
}