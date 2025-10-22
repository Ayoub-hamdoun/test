#include "../includes/JoinCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>


JoinCommand::JoinCommand(Server* server) : BaseCommand(server) {}
JoinCommand::~JoinCommand() {}

void JoinCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, 2, ":You have not registered");
        return;
    }
    
    if (args.empty()) {
        sendError(client, 461, "JOIN :Not enough parameters");
        return;
    }
    
    std::string channelName = args[0];
    std::string channelKey = args.size() > 1 ? args[1] : "";
    
    // Validate channel name
    if (channelName.empty() || channelName[0] != '#' || channelName.length() > 50) {
        sendError(client, 403, channelName + " :No such channel");
        return;
    }
    
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        // Create new channel
        channel = _server->createChannel(channelName, client);
        std::cout << "Created new channel: " << channelName << std::endl;
    }
    
    // Check if client can join
    if (channel->isInviteOnly() && !channel->isInvited(client)) {
        sendError(client, 473, channelName + " :Cannot join channel (+i)");
        return;
    }
    
    if (!channel->getKey().empty() && channel->getKey() != channelKey) {
        sendError(client, 475, channelName + " :Cannot join channel (+k)");
        return;
    }
    
    // FIXED: Cast getUserLimit() to size_t for comparison
    if (channel->getUserLimit() > 0 && channel->getClientCount() >= static_cast<size_t>(channel->getUserLimit())) {
        sendError(client, 471, channelName + " :Cannot join channel (+l)");
        return;
    }
    
    // Add client to channel
    channel->addClient(client);
    if (channel->getClientCount() == 1) {
        channel->setOperator(client, true); // First user becomes operator
    }
    
    // Send join message to all channel members
    std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(joinMsg);
    
    // Send topic if set
    channel->sendTopic(client);
    
    // Send names list
    channel->sendNamesList(client);
    
    std::cout << "Client " << client->getNickname() << " joined channel " << channelName << std::endl;
}