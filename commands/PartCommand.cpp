#include "../includes/PartCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>

PartCommand::PartCommand(Server* server) : BaseCommand(server) {}
PartCommand::~PartCommand() {}

void PartCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, "451", ":You have not registered");
        return;
    }
    
    if (args.empty()) {
        sendError(client, "461", "PART :Not enough parameters");
        return;
    }
    
    std::string channelName = args[0];
    std::string reason = args.size() > 1 ? args[1] : "Leaving";
    
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        sendError(client, "403", channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasClient(client)) {
        sendError(client, "442", channelName + " :You're not on that channel");
        return;
    }
    
    std::string partMsg = ":" + client->getUserMask() + " PART " + channelName + " :" + reason + "\r\n";
    channel->broadcast(partMsg);
    
    channel->removeClient(client);
    
    std::cout << "Client " << client->getNickname() << " left channel " << channelName << std::endl;
}