#include "../includes/JoinCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <sstream>

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
    
    // Split channels and keys
    std::vector<std::string> channels = splitByComma(args[0]);
    std::vector<std::string> keys;
    
    if (args.size() > 1) {
        keys = splitByComma(args[1]);
    }
    
    // Join each channel
    for (size_t i = 0; i < channels.size(); ++i) {
        std::string channelKey = "";
        
        // Match channel with its key (if available)
        if (i < keys.size()) {
            channelKey = keys[i];
        }
        
        joinSingleChannel(client, channels[i], channelKey);
    }


    // if (!client->isRegistered()) {
    //     sendError(client, "451", ":You have not registered");
    //     return;
    // }
    
    // if (args.empty()) {
    //     sendError(client, "461", "JOIN :Not enough parameters");
    //     return;
    // }
    
    // std::string channelName = args[0];
    // std::string channelKey = args.size() > 1 ? args[1] : "";
    
    // if (channelName.empty() || channelName[0] != '#'  || channelName.length() > 50) {
    //     sendError(client, "403", channelName + " :No such channel");
    //     return;
    // }
    
    // Channel* channel = _server->getChannel(channelName);
    // if (!channel) {
    //     channel = _server->createChannel(channelName, client);
    //     std::cout << "Created new channel: " << channelName << std::endl;
    // }
    
    // if (channel->isInviteOnly() && !channel->isInvited(client)) {
    //     sendError(client, "473", channelName + " :Cannot join channel (+i)");
    //     return;
    // }
    
    // if (!channel->getKey().empty() && channel->getKey() != channelKey) {
    //     sendError(client, "475", channelName + " :Cannot join channel (+k)");
    //     return;
    // }
    
    // if (channel->getUserLimit() > 0 && channel->getClientCount() >= static_cast<size_t>(channel->getUserLimit())) {
    //     sendError(client, "471", channelName + " :Cannot join channel (+l)");
    //     return;
    // }
    
    // channel->addClient(client);
    // if (channel->getClientCount() == 1) {
    //     channel->setOperator(client, true);
    // }
    
    // channel->removeInvited(client);
    
    // std::string joinMsg = ":" + client->getUserMask() + " JOIN " + channelName + "\r\n";
    // channel->broadcast(joinMsg);
    
    // channel->sendTopic(client, _server->getServerName());
    // channel->sendNamesList(client, _server->getServerName());
    
    // std::cout << "Client " << client->getNickname() << " joined channel " << channelName << std::endl;
}


std::vector<std::string> JoinCommand::splitByComma(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // Remove whitespace
        size_t start = item.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        
        size_t end = item.find_last_not_of(" \t");
        item = item.substr(start, end - start + 1);
        
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}


void JoinCommand::joinSingleChannel(Client* client, const std::string& channelName, const std::string& channelKey) {
    // Validate channel name
    if (channelName.empty() || channelName[0] != '#' || channelName.length() <= 1 || channelName.length() > 50) {
        sendError(client, "403", channelName + " :No such channel");
        return;
    }
    
    // Get or create channel
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        channel = _server->createChannel(channelName, client);
        std::cout << "Created new channel: " << channelName << std::endl;
    }
    
    // Check if already in channel
    if (channel->hasClient(client)) {
        return; // Silently ignore if already joined
    }
    
    // Check invite-only
    if (channel->isInviteOnly() && !channel->isInvited(client)) {
        sendError(client, "473", channelName + " :Cannot join channel (+i)");
        return;
    }
    
    // Check key/password
    if (!channel->getKey().empty() && channel->getKey() != channelKey) {
        sendError(client, "475", channelName + " :Cannot join channel (+k)");
        return;
    }
    
    // Check user limit
    if (channel->getUserLimit() > 0 && channel->getClientCount() >= static_cast<size_t>(channel->getUserLimit())) {
        sendError(client, "471", channelName + " :Cannot join channel (+l)");
        return;
    }
    
    // Add client to channel
    channel->addClient(client);
    if (channel->getClientCount() == 1) {
        channel->setOperator(client, true);
    }
    
    channel->removeInvited(client);
    
    // Broadcast JOIN message
    std::string joinMsg = ":" + client->getUserMask() + " JOIN " + channelName + "\r\n";
    channel->broadcast(joinMsg);
    
    // Send topic and names list
    channel->sendTopic(client, _server->getServerName());
    channel->sendNamesList(client, _server->getServerName());
    
    std::cout << "Client " << client->getNickname() << " joined channel " << channelName << std::endl;
}

