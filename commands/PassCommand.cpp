#include "../includes/PassCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>

PassCommand::PassCommand(Server* server) : BaseCommand(server) {}
PassCommand::~PassCommand() {}

void PassCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (client->isAuthenticated()) {
        sendError(client, 462, ":You may not reregister");
        return;
    }
    
    if (args.empty()) {
        sendError(client, 461, "PASS :Not enough parameters");
        return;
    }
    
    if (_server->validatePassword(args[0])) {
        client->setAuthenticated(true);
        std::cout << "Client authenticated successfully" << std::endl;
        
        // Check if client can be fully registered now (if NICK and USER were already set)
        if (!client->isRegistered() && client->isAuthenticated() && 
            !client->getNickname().empty() && !client->getUsername().empty()) {
            client->setRegistered(true);
            // We need to send welcome from here since this might complete registration
            std::string welcomeMsg = ":" + _server->getServerName() + " 001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getPrefix() + "\r\n";
            _server->sendToClient(client->getFd(), welcomeMsg);
            std::cout << "Client " << client->getNickname() << " successfully registered via PASS" << std::endl;
        }
    } else {
        sendError(client, 464, ":Password incorrect");
        _server->sendToClient(client->getFd(), "ERROR :Bad password\r\n");
    }
}