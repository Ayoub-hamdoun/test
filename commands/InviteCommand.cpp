#include "../includes/InviteCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>

InviteCommand::InviteCommand(Server* server) : BaseCommand(server) {}
InviteCommand::~InviteCommand() {}

void InviteCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, 1, ":You have not registered");
        return;
    }
    
    if (args.size() < 2) {
        sendError(client, 461, "INVITE :Not enough parameters");
        return;
    }
    
    std::string targetNick = args[0];
    std::string channelName = args[1];
    
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        sendError(client, 403, channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasClient(client)) {
        sendError(client, 442, channelName + " :You're not on that channel");
        return;
    }
    
    if (channel->isInviteOnly() && !channel->isOperator(client)) {
        sendError(client, 482, channelName + " :You're not channel operator");
        return;
    }
    
    Client* targetClient = _server->getClient(targetNick);
    if (!targetClient) {
        sendError(client, 401, targetNick + " :No such nick");
        return;
    }
    
    if (channel->hasClient(targetClient)) {
        sendError(client, 443, targetNick + " " + channelName + " :is already on channel");
        return;
    }
    
    // Add to invited list and send invite
    channel->addInvited(targetClient);
    
    // Send invite notification to target client
    std::string inviteMsg = ":" + client->getPrefix() + " INVITE " + targetNick + " :" + channelName + "\r\n";
    _server->sendToClient(targetClient->getFd(), inviteMsg);
    
    // Send confirmation to inviting client
    sendReply(client, "341 " + client->getNickname() + " " + targetNick + " " + channelName);
    
    std::cout << "Client " << client->getNickname() << " invited " << targetNick << " to " << channelName << std::endl;
}