#include "../includes/Commands.hpp"
#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/PassCommand.hpp"
#include "../includes/UserCommand.hpp"
#include "../includes/JoinCommand.hpp"
#include "../includes/PrivMsgCommand.hpp"
#include "../includes/KickCommand.hpp"
#include "../includes/NickCommand.hpp"
#include "../includes/InviteCommand.hpp"
#include "../includes/TopicCommand.hpp"
#include "../includes/ModeCommand.hpp"
#include "../includes/QuitCommand.hpp"
#include "../includes/PartCommand.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

Commands::Commands(Server* server) : _server(server) {}
Commands::~Commands() {}

void Commands::execute(Client* client, const std::string& commandLine) {
    std::string cleanLine = commandLine;
    if (!cleanLine.empty() && cleanLine[cleanLine.length()-1] == '\r') {
        cleanLine = cleanLine.substr(0, cleanLine.length()-1);
    }
    
    std::cout << "DEBUG RAW: '" << cleanLine << "'" << std::endl;
    
    std::stringstream ss(cleanLine);
    std::string command;
    std::vector<std::string> args;
    std::string token;
    
    ss >> command;
    
    while (ss >> token) {
        // std::cout << "--------> " << token << std::endl;
        if (token[0] == ':') {
            std::string trailing = token.substr(1);
            std::string rest;
            std::getline(ss, rest);
            // std::cout << "=======>|" << token << "|--|" << trailing << "|--|" << rest << "|" << std::endl;

            trailing += rest;

            // std::cout << "=======>|" << token << "|--|" << trailing << "|--|" << rest << "|" << std::endl;
            args.push_back(trailing);
            break;
        } else {
            args.push_back(token);
        }
    }
    
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    
    if (command == "PASS") {
        PassCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "NICK") {
        NickCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "USER") {
        UserCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "JOIN") {
        JoinCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "PRIVMSG") {
        PrivMsgCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "KICK") {
        KickCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "INVITE") {
        InviteCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "TOPIC") {
        TopicCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "MODE") {
        ModeCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "QUIT") {
        QuitCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "PART") {
        PartCommand cmd(_server);
        cmd.execute(client, args);
    }
    else if (command == "PING") {
        std::string pongMsg = ":" + _server->getServerName() + " PONG " + _server->getServerName();
        if (!args.empty()) {
            pongMsg += " :" + args[0];
        }
        pongMsg += "\r\n";
        _server->sendToClient(client->getFd(), pongMsg);
    }
    else if (command == "PONG") { // check this
    }
    else if (command == "CAP") {
        if (!args.empty() && args[0] == "LS") {
            _server->sendToClient(client->getFd(), "CAP * LS :\r\n");
        }
        else if (!args.empty() && args[0] == "END") {
        }
    }
    else {
        std::string response = ":" + _server->getServerName() + " 421 " + client->getNickname() + " " + command + " :Unknown command\r\n";
        _server->sendToClient(client->getFd(), response);
    }
}