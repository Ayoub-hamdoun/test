#include "../includes/ModeCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cstdlib>

ModeCommand::ModeCommand(Server* server) : BaseCommand(server) {}
ModeCommand::~ModeCommand() {}

void ModeCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, "451", ":You have not registered");
        return;
    }
    
    if (args.empty()) {
        sendError(client, "461", "MODE :Not enough parameters");
        return;
    }
    
    std::string target = args[0];
    
    if (target[0] == '#' || target[0] == '&') {
        handleChannelMode(client, args);
    } else {
        sendError(client, "502", ":Cannot change mode for other users");
    }
}

void ModeCommand::handleChannelMode(Client* client, const std::vector<std::string>& args) {
    std::string channelName = args[0];
    Channel* channel = _server->getChannel(channelName);
    
    if (!channel) {
        sendError(client, "403", channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasClient(client)) {
        sendError(client, "442", channelName + " :You're not on that channel");
        return;
    }
    
    if (args.size() == 1) {
        channel->sendChannelMode(client, _server->getServerName());
        return;
    }
    
    if (!channel->isOperator(client)) {
        sendError(client, "482", channelName + " :You're not channel operator");
        return;
    }
    
    std::string modes = args[1];
    bool adding = true;
    size_t argIndex = 2;
    
    for (size_t i = 0; i < modes.length(); ++i) {
        char mode = modes[i];
        
        if (mode == '+') {
            adding = true;
            continue;
        } else if (mode == '-') {
            adding = false;
            continue;
        }
        
        std::string broadcastMsg;
        
        switch (mode) {
            case 'i':
                channel->setInviteOnly(adding);
                broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " " + (adding ? "+i" : "-i") + "\r\n";
                break;
                
            case 't':
                channel->setTopicRestricted(adding);
                broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " " + (adding ? "+t" : "-t") + "\r\n";
                break;
                
            case 'k':
                if (adding) {
                    if (argIndex < args.size()) {
                        channel->setKey(args[argIndex]);
                        broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " +k " + args[argIndex] + "\r\n";
                        argIndex++;
                    }
                } else {
                    channel->setKey("");
                    broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " -k\r\n";
                }
                break;
                
            case 'o':
                if (argIndex < args.size()) {
                    Client* targetClient = _server->getClient(args[argIndex]);
                    if (targetClient && channel->hasClient(targetClient)) {
                        channel->setOperator(targetClient, adding);
                        broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " " + (adding ? "+o " : "-o ") + args[argIndex] + "\r\n";
                    }
                    argIndex++;
                }
                break;
                
            case 'l':
                if (adding) {
                    if (argIndex < args.size()) {
                        int limit = atoi(args[argIndex].c_str());
                        if (limit > 0) {
                            channel->setUserLimit(limit);
                            broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " +l " + args[argIndex] + "\r\n";
                        }
                        argIndex++;
                    }
                } else {
                    channel->setUserLimit(0);
                    broadcastMsg = ":" + client->getUserMask() + " MODE " + channelName + " -l\r\n";
                }
                break;
                
            default:
                sendError(client, "472", std::string(1, mode) + " :is unknown mode char to me");
                continue;
        }
        
        if (!broadcastMsg.empty()) {
            channel->broadcast(broadcastMsg);
        }
    }
}