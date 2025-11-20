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
    
    std::vector<std::string> channels = splitByComma(args[0]);
    std::vector<std::string> keys;
    
    if (args.size() > 1) {
        keys = splitByComma(args[1]);
    }
    
    for (size_t i = 0; i < channels.size(); ++i) {
        std::string channelKey = "";
        
        if (i < keys.size()) {
            channelKey = keys[i];
        }
        
        joinSingleChannel(client, channels[i], channelKey);
    }
}


std::vector<std::string> JoinCommand::splitByComma(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
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
    if (channelName.empty() || channelName[0] != '#' || channelName.length() <= 1 || channelName.length() > 50) {
        sendError(client, "403", channelName + " :No such channel");
        return;
    }

    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        channel = _server->createChannel(channelName, client);
        std::cout << "Created new channel: " << channelName << std::endl;
    }
    
    if (channel->hasClient(client)) {
        return;
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

