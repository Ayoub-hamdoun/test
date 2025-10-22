#include "../includes/KickCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>


KickCommand::KickCommand(Server* server) : BaseCommand(server) {}
KickCommand::~KickCommand() {}

void KickCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, 451, ":You have not registered");
        return;
    }
    
    if (args.size() < 2) {
        sendError(client, 461, "KICK :Not enough parameters");
        return;
    }
    
    std::string channelName = args[0];
    std::string targetNick = args[1];
    std::string reason = args.size() > 2 ? args[2] : client->getNickname();
    
    // Find the channel
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        sendError(client, 403, channelName + " :No such channel");
        return;
    }
    
    // Check if client is in the channel
    if (!channel->hasClient(client)) {
        sendError(client, 442, channelName + " :You're not on that channel");
        return;
    }
    
    // Check if client is channel operator
    if (!channel->isOperator(client)) {
        sendError(client, 482, channelName + " :You're not channel operator");
        return;
    }
    
    // Find the target client
    Client* targetClient = _server->getClient(targetNick);
    if (!targetClient) {
        sendError(client, 401, targetNick + " :No such nick");
        return;
    }
    
    // Check if target is in the channel
    if (!channel->hasClient(targetClient)) {
        sendError(client, 441, targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }
    
    // Cannot kick yourself
    if (targetClient == client) {
        sendError(client, 482, channelName + " :You cannot kick yourself");
        return;
    }
    
    // Broadcast kick message to the channel
    std::string kickMsg = ":" + client->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
    channel->broadcast(kickMsg);
    
    // Remove client from channel
    channel->removeClient(targetClient);
    
    std::cout << "Client " << targetNick << " was kicked from " << channelName << " by " << client->getNickname() << " - Reason: " << reason << std::endl;
}