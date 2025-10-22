#include "../includes/BaseCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
// ... rest of the code
#include <sstream>

BaseCommand::BaseCommand(Server* server) : _server(server) {}
BaseCommand::~BaseCommand() {}

void BaseCommand::sendError(Client* client, int code, const std::string& message) {
    std::string errorMsg = ":" + _server->getServerName() + " " + toString(code) + " " + client->getNickname() + " " + message + "\r\n";
    _server->sendToClient(client->getFd(), errorMsg);
}

void BaseCommand::sendReply(Client* client, const std::string& reply) {
    std::string replyMsg = ":" + _server->getServerName() + " " + reply + "\r\n";
    _server->sendToClient(client->getFd(), replyMsg);
}

std::string BaseCommand::toString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}