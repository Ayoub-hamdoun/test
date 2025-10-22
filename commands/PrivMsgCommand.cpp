
#include "../includes/PrivMsgCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>

PrivMsgCommand::PrivMsgCommand(Server* server) : BaseCommand(server) {}
PrivMsgCommand::~PrivMsgCommand() {}

void PrivMsgCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, 6, ":You have not registered");
        return;
    }
    
    if (args.size() < 2) {
        sendError(client, 411, ":No recipient given (PRIVMSG)");
        return;
    }
    
    std::string target = args[0];
    std::string message = args[1];
    
    if (target[0] == '#' || target[0] == '&') {
        // Channel message
        Channel* channel = _server->getChannel(target);
        if (!channel) {
            sendError(client, 403, target + " :No such channel");
            return;
        }
        
        if (!channel->hasClient(client)) {
            sendError(client, 404, target + " :Cannot send to channel");
            return;
        }
        
        std::string privMsg = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
        channel->broadcast(privMsg, client);
        std::cout << "Message from " << client->getNickname() << " to channel " << target << ": " << message << std::endl;
    } else {
        // Private message to user
        Client* targetClient = _server->getClient(target);
        if (!targetClient) {
            sendError(client, 401, target + " :No such nick/channel");
            return;
        }
        
        std::string privMsg = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
        _server->sendToClient(targetClient->getFd(), privMsg);
        std::cout << "Private message from " << client->getNickname() << " to " << target << ": " << message << std::endl;
    }
}