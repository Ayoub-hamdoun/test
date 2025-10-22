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
        sendError(client, 4, ":You have not registered");
        return;
    }
    
    if (args.empty()) {
        sendError(client, 461, "MODE :Not enough parameters");
        return;
    }
    
    std::string target = args[0];
    
    if (target[0] == '#' || target[0] == '&') {
        handleChannelMode(client, args);
    } else {
        // User modes (not required by subject)
        sendError(client, 502, ":Cannot change mode for other users");
    }
}

void ModeCommand::handleChannelMode(Client* client, const std::vector<std::string>& args) {
    std::string channelName = args[0];
    Channel* channel = _server->getChannel(channelName);
    
    if (!channel) {
        sendError(client, 403, channelName + " :No such channel");
        return;
    }
    
    if (!channel->hasClient(client)) {
        sendError(client, 442, channelName + " :You're not on that channel");
        return;
    }
    
    if (args.size() == 1) {
        // Query current modes
        std::string modes = "+";
        if (channel->isInviteOnly()) modes += "i";
        if (channel->isTopicRestricted()) modes += "t";
        if (!channel->getKey().empty()) modes += "k";
        if (channel->getUserLimit() > 0) modes += "l";
        
        sendReply(client, "324 " + client->getNickname() + " " + channelName + " " + modes);
        return;
    }
    
    if (!channel->isOperator(client)) {
        sendError(client, 482, channelName + " :You're not channel operator");
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
        
        std::string modeChange;
        
        switch (mode) {
            case 'i': // Invite-only
                channel->setInviteOnly(adding);
                modeChange = (adding ? "+i" : "-i");
                break;
                
            case 't': // Topic restriction
                channel->setTopicRestricted(adding);
                modeChange = (adding ? "+t" : "-t");
                break;
                
            case 'k': // Channel key
                if (adding) {
                    if (argIndex < args.size()) {
                        channel->setKey(args[argIndex]);
                        modeChange = "+k " + args[argIndex];
                        argIndex++;
                    }
                } else {
                    channel->setKey("");
                    modeChange = "-k";
                }
                break;
                
            case 'o': // Operator privilege
                if (argIndex < args.size()) {
                    Client* targetClient = _server->getClient(args[argIndex]);
                    if (targetClient && channel->hasClient(targetClient)) {
                        channel->setOperator(targetClient, adding);
                        modeChange = (adding ? "+o " : "-o ") + args[argIndex];
                    }
                    argIndex++;
                }
                break;
                
            case 'l': // User limit
                if (adding) {
                    if (argIndex < args.size()) {
                        int limit = atoi(args[argIndex].c_str());
                        if (limit > 0) {
                            channel->setUserLimit(limit);
                            modeChange = "+l " + args[argIndex];
                        }
                        argIndex++;
                    }
                } else {
                    channel->setUserLimit(0);
                    modeChange = "-l";
                }
                break;
                
            default:
                // Unknown mode
                continue;
        }
        
        // Broadcast mode change
        if (!modeChange.empty()) {
            std::string modeMsg = ":" + client->getPrefix() + " MODE " + channelName + " " + modeChange + "\r\n";
            channel->broadcast(modeMsg);
        }
    }
}