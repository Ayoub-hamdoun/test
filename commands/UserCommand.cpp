#include "../includes/UserCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>



UserCommand::UserCommand(Server* server) : BaseCommand(server) {}
UserCommand::~UserCommand() {}

void UserCommand::execute(Client* client, const std::vector<std::string>& args) {
    // FIXED: Remove all checks - USER should work during registration
    if (client->isRegistered()) {
        sendError(client, 462, ":You may not reregister");
        return;
    }
    
    if (args.size() < 4) {
        sendError(client, 461, "USER :Not enough parameters");
        return;
    }
    
    client->setUsername(args[0]);
    client->setRealname(args[3]);
    std::cout << "DEBUG: Client set username to: " << args[0] << " and realname to: " << args[3] << std::endl;
    std::cout << "DEBUG: Client auth: " << client->isAuthenticated() << " registered: " << client->isRegistered() << std::endl;
    
    // Check if client can be fully registered now (only if not already registered)
    if (!client->isRegistered() && client->isAuthenticated() && 
        !client->getNickname().empty() && !client->getUsername().empty()) {
        client->setRegistered(true);
        sendWelcome(client);
    }
}

void UserCommand::sendWelcome(Client* client) {
    sendReply(client, "001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getPrefix());
    sendReply(client, "002 " + client->getNickname() + " :Your host is " + _server->getServerName() + ", running version 1.0");
    sendReply(client, "003 " + client->getNickname() + " :This server was created today");
    sendReply(client, "004 " + client->getNickname() + " " + _server->getServerName() + " 1.0 o o");
    std::cout << "Client " << client->getNickname() << " successfully registered" << std::endl;
}