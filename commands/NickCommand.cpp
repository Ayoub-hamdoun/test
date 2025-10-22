#include "../includes/NickCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <cctype>
#include <cstring>


NickCommand::NickCommand(Server* server) : BaseCommand(server) {}
NickCommand::~NickCommand() {}

bool NickCommand::isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 9) {
        return false;
    }
    
    // First character must be a letter
    if (!std::isalpha(nick[0])) {
        return false;
    }
    
    // Remaining characters can be letters, digits, or hyphens
    for (size_t i = 1; i < nick.length(); ++i) {
        if (!std::isalnum(nick[i]) && nick[i] != '-') {
            return false;
        }
    }
    
    return true;
}

void NickCommand::execute(Client* client, const std::vector<std::string>& args) {
    // FIXED: Only check if client is already registered and trying to use NICK during registration
    // Allow NICK in all cases - it's part of the registration process
    if (args.empty()) {
        sendError(client, 431, ":No nickname given");
        return;
    }
    
    std::string newNick = args[0];
    
    // Validate nickname format
    if (!isValidNickname(newNick)) {
        sendError(client, 432, newNick + " :Erroneous nickname");
        return;
    }
    
    // Check if nickname is already in use
    Client* existingClient = _server->getClient(newNick);
    if (existingClient != NULL && existingClient != client) {
        sendError(client, 433, newNick + " :Nickname is already in use");
        return;
    }
    
    std::string oldNick = client->getNickname();
    
    // Set the new nickname
    client->setNickname(newNick);
    std::cout << "DEBUG: Client set nickname to: " << newNick << std::endl;
    std::cout << "DEBUG: Client auth: " << client->isAuthenticated() << " registered: " << client->isRegistered() << std::endl;
    
    // Check if client can be fully registered now (only if not already registered)
    if (!client->isRegistered() && client->isAuthenticated() && 
        !client->getNickname().empty() && !client->getUsername().empty()) {
        client->setRegistered(true);
        sendWelcome(client);
    }
    
    // Broadcast nickname change if client was already registered and actually changed nick
    if (client->isRegistered() && !oldNick.empty() && oldNick != newNick) {
        std::string broadcast = ":" + oldNick + " NICK :" + newNick + "\r\n";
        
        // Broadcast to all channels the client is in
        std::vector<Channel*> channels = _server->getClientChannels(client);    
        for (size_t i = 0; i < channels.size(); ++i) {
            channels[i]->broadcast(broadcast);
        }
        std::cout << "Client changed nickname from " << oldNick << " to " << newNick << std::endl;
    }
}

void NickCommand::sendWelcome(Client* client) {
    sendReply(client, "001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getPrefix());
    sendReply(client, "002 " + client->getNickname() + " :Your host is " + _server->getServerName() + ", running version 1.0");
    sendReply(client, "003 " + client->getNickname() + " :This server was created today");
    sendReply(client, "004 " + client->getNickname() + " " + _server->getServerName() + " 1.0 o o");
    std::cout << "Client " << client->getNickname() + " successfully registered" << std::endl;
}