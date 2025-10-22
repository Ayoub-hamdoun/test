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
// ... rest of the code
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

Commands::Commands(Server* server) : _server(server) {}
Commands::~Commands() {}

void Commands::execute(Client* client, const std::string& commandLine) {
    // DEBUG: Show what we're receiving
    std::cout << "DEBUG RAW: '" << commandLine << "'" << std::endl;
    
    std::stringstream ss(commandLine);
    std::string command;
    std::vector<std::string> args;
    std::string token;
    
    ss >> command;
    
    // Parse arguments with proper trailing parameter handling
    while (ss >> token) {
        if (token[0] == ':') {
            // Found the trailing parameter - get everything after colon as one string
            std::string trailing = token.substr(1);  // Remove the colon
            
            // Get the rest of the line
            std::string rest;
            std::getline(ss, rest);
            
            // Combine and trim leading space if present
            if (!rest.empty() && rest[0] == ' ') {
                trailing += rest;
            } else if (!rest.empty()) {
                trailing += " " + rest;
            }
            
            args.push_back(trailing);
            break;  // Stop parsing after trailing parameter
        } else {
            args.push_back(token);
        }
    }
    
    // DEBUG: Show parsed result
    std::cout << "DEBUG PARSED - Command: '" << command << "' Args: " << args.size() << std::endl;
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << "  [" << i << "] = '" << args[i] << "'" << std::endl;
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
    else {
        std::string response = ":" + _server->getServerName() + " 421 " + client->getNickname() + " " + command + " :Unknown command\r\n";
        _server->sendToClient(client->getFd(), response);
    }
}