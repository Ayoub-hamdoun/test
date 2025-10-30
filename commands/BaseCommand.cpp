#include "../includes/BaseCommand.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include <sstream>

BaseCommand::BaseCommand(Server* server) : _server(server) {}
BaseCommand::~BaseCommand() {}

void BaseCommand::sendError(Client* client, const std::string& code, const std::string& message) {
    std::string errorMsg = ":" + _server->getServerName() + " " + code + " " + client->getNickname() + " " + message + "\r\n";
    _server->sendToClient(client->getFd(), errorMsg);
}

void BaseCommand::sendReply(Client* client, const std::string& code, const std::string& message) {
    std::string replyMsg = ":" + _server->getServerName() + " " + code + " " + client->getNickname() + " " + message + "\r\n";
    _server->sendToClient(client->getFd(), replyMsg);
}

void BaseCommand::sendRaw(Client* client, const std::string& message) {
    std::string formattedMsg = message;
    if (message.length() < 2 || message.substr(message.length() - 2) != "\r\n") {
        formattedMsg += "\r\n";
    }
    _server->sendToClient(client->getFd(), formattedMsg);
}

std::string BaseCommand::toString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}