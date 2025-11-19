#include "../includes/PrivMsgCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

PrivMsgCommand::PrivMsgCommand(Server* server) : BaseCommand(server) {}
PrivMsgCommand::~PrivMsgCommand() {}

void PrivMsgCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (!client->isRegistered()) {
        sendError(client, "451", ":You have not registered");
        return;
    }
    
    if (args.size() < 2) {
        sendError(client, "411", ":No recipient given (PRIVMSG)");
        return;
    }
    
    if (args[1].empty()) {
        sendError(client, "412", ":No text to send");
        return;
    }
    
    std::string targetsStr = args[0];
    std::string message = args[1];
    
    // Split multiple receivers by comma
    std::vector<std::string> targets = splitTargets(targetsStr);
    
    if (targets.empty()) {
        sendError(client, "411", ":No recipient given (PRIVMSG)");
        return;
    }
    
    // Remove duplicate targets silently (more user-friendly)
    std::vector<std::string> uniqueTargets;
    for (size_t i = 0; i < targets.size(); ++i) {
        if (std::find(uniqueTargets.begin(), uniqueTargets.end(), targets[i]) == uniqueTargets.end()) {
            uniqueTargets.push_back(targets[i]);
        }
    }
    
    bool anySuccess = false;
    std::string failedTargets;
    
    for (size_t i = 0; i < uniqueTargets.size(); ++i) {
        bool success = sendToTarget(client, uniqueTargets[i], message);
        if (success) {
            anySuccess = true;
        } else {
            if (!failedTargets.empty()) failedTargets += ", ";
            failedTargets += uniqueTargets[i];
        }
    }
    
    // If all targets failed, send error
    if (!anySuccess) {
        sendError(client, "401", failedTargets + " :No such nick/channel");
    }
}

std::vector<std::string> PrivMsgCommand::splitTargets(const std::string& targetsStr) {
    std::vector<std::string> targets;
    std::stringstream ss(targetsStr);
    std::string target;
    
    while (std::getline(ss, target, ',')) {
        // Remove leading/trailing whitespace
        size_t start = target.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        
        size_t end = target.find_last_not_of(" \t");
        target = target.substr(start, end - start + 1);
        
        if (!target.empty()) {
            targets.push_back(target);
        }
    }
    
    return targets;
}

bool PrivMsgCommand::sendToTarget(Client* client, const std::string& target, const std::string& message) {
    if (target[0] == '#') {
        return sendToChannel(client, target, message);
    } else {
        return sendToUser(client, target, message);
    }
}

bool PrivMsgCommand::sendToChannel(Client* client, const std::string& channelName, const std::string& message) {
    Channel* channel = _server->getChannel(channelName);
    if (!channel) {
        return false;
    }
    
    if (!channel->hasClient(client)) {
        sendError(client, "404", channelName + " :Cannot send to channel");
        return false;
    }
    
    std::string privMsg = ":" + client->getUserMask() + " PRIVMSG " + channelName + " :" + message + "\r\n";
    channel->broadcast(privMsg, client);
    std::cout << "Message from " << client->getNickname() << " to channel " << channelName << ": " << message << std::endl;
    return true;
}

bool PrivMsgCommand::sendToUser(Client* client, const std::string& nickname, const std::string& message) {
    Client* targetClient = _server->getClient(nickname);
    if (!targetClient) {
        return false;
    }
    
    std::string privMsg = ":" + client->getUserMask() + " PRIVMSG " + nickname + " :" + message + "\r\n";
    _server->sendToClient(targetClient->getFd(), privMsg);
    std::cout << "Private message from " << client->getNickname() << " to " << nickname << ": " << message << std::endl;
    return true;
}