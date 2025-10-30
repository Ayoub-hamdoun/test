#include "../includes/QuitCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>

QuitCommand::QuitCommand(Server* server) : BaseCommand(server) {}
QuitCommand::~QuitCommand() {}

void QuitCommand::execute(Client* client, const std::vector<std::string>& args) {
    std::string reason = args.empty() ? "Client quit" : args[0];
    
    std::string quitMsg = ":" + client->getUserMask() + " QUIT :Quit: " + reason + "\r\n";
    
    std::vector<Channel*> channels = _server->getClientChannels(client);
    for (size_t i = 0; i < channels.size(); ++i) {
        channels[i]->broadcast(quitMsg, client);
        channels[i]->removeClient(client);
    }
    
    client->setRegistered(false);
    
    std::cout << "Client " << client->getNickname() << " quit: " << reason << std::endl;
}