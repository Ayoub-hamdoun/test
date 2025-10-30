#include "../includes/TopicCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>

TopicCommand::TopicCommand(Server* server) : BaseCommand(server) {}
TopicCommand::~TopicCommand() {}

void TopicCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, "451", ":You have not registered");
        return;
    }
    
    if (args.empty()) {
        sendError(client, "461", "TOPIC :Not enough parameters");
        return;
    }
    
    std::string channelName = args[0];
    std::string newTopic = args.size() > 1 ? args[1] : "";
    
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        sendError(client, "403", channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasClient(client)) {
        sendError(client, "442", channelName + " :You're not on that channel");
        return;
    }
    
    if (newTopic.empty()) {
        channel->sendTopic(client, _server->getServerName());
    } else {
        if (channel->isTopicRestricted() && !channel->isOperator(client)) {
            sendError(client, "482", channelName + " :You're not channel operator");
            return;
        }
        
        channel->setTopic(newTopic);
        
        std::string topicMsg = ":" + client->getUserMask() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        channel->broadcast(topicMsg);
        
        std::cout << "Topic for " << channelName << " set to: " << newTopic << " by " << client->getNickname() << std::endl;
    }
}