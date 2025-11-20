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
    if (nick.empty() || nick.length() > 30) {
        return false;
    }
    
    if (!std::isalpha(nick[0]) && nick[0] != '[' && nick[0] != ']' && 
        nick[0] != '\\' && nick[0] != '`' && nick[0] != '_' && 
        nick[0] != '^' && nick[0] != '{' && nick[0] != '|' && nick[0] != '}') {
        return false;
    }
    
    for (size_t i = 1; i < nick.length(); ++i) {
        if (!std::isalnum(nick[i]) && nick[i] != '-' && nick[i] != '[' && 
            nick[i] != ']' && nick[i] != '\\' && nick[i] != '`' && 
            nick[i] != '_' && nick[i] != '^' && nick[i] != '{' && 
            nick[i] != '|' && nick[i] != '}') {
            return false;
        }
    }
    
    return true;
}

void NickCommand::execute(Client* client, const std::vector<std::string>& args) {

    if (!client->isAuthenticated()) {
        sendError(client, "451", ":You have not registered (use PASS first)");
        return;
    }

    if (args.empty()) {
        sendError(client, "431", ":No nickname given");
        return;
    }
    
    std::string newNick = args[0];
    
    if (!isValidNickname(newNick)) {
        sendError(client, "432", newNick + " :Erroneous nickname");
        return;
    }
    
    Client* existingClient = _server->getClient(newNick);
    if (existingClient != NULL && existingClient != client) {
        sendError(client, "433", newNick + " :Nickname is already in use");
        return;
    }
    
    std::string oldNick = client->getNickname();
    client->setNickname(newNick);
    
    if (oldNick.empty()) {
        if (!client->isRegistered() && client->isAuthenticated() && 
            !client->getNickname().empty() && !client->getUsername().empty()) {
            client->setRegistered(true);
            _server->sendWelcome(client);
        }
    } else {
        std::string broadcast = ":" + oldNick + "!" + client->getUsername() + "@" + client->getHostname() + " NICK :" + newNick + "\r\n";
        std::vector<Channel*> channels = _server->getClientChannels(client);    
        for (size_t i = 0; i < channels.size(); ++i) {
            channels[i]->broadcast(broadcast);
        }
        std::cout << "Client changed nickname from " << oldNick << " to " << newNick << std::endl;
    }
}