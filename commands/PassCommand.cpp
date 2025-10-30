#include "../includes/PassCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <iostream>

PassCommand::PassCommand(Server* server) : BaseCommand(server) {}
PassCommand::~PassCommand() {}

void PassCommand::execute(Client* client, const std::vector<std::string>& args) {
    if (client->isAuthenticated()) {
        sendError(client, "462", ":You may not reregister");
        return;
    }
    
    if (args.empty()) {
        sendError(client, "461", "PASS :Not enough parameters");
        return;
    }
    
    if (_server->validatePassword(args[0])) {
        client->setAuthenticated(true);
        std::cout << "Client authenticated successfully" << std::endl;
    } else {
        sendError(client, "464", ":Password incorrect");
        _server->sendToClient(client->getFd(), "ERROR :Bad password\r\n");
        client->setRegistered(false);
    }
}