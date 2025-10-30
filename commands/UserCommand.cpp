#include "../includes/UserCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>

UserCommand::UserCommand(Server* server) : BaseCommand(server) {}
UserCommand::~UserCommand() {}

void UserCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (client->isRegistered()) {
        sendError(client, "462", ":You may not reregister");
        return;
    }
    
    if (args.size() < 4) {
        sendError(client, "461", "USER :Not enough parameters");
        return;
    }
    
    if (!client->getUsername().empty()) {
        sendError(client, "462", ":You may not reregister");
        return;
    }
    
    client->setUsername(args[0]);
    client->setRealname(args[3]);
    
    std::cout << "DEBUG: Client set username to: " << args[0] << " and realname to: " << args[3] << std::endl;
    
    if (!client->isRegistered() && client->isAuthenticated() && 
        !client->getNickname().empty() && !client->getUsername().empty()) {
        client->setRegistered(true);
        _server->sendWelcome(client);
    }
}