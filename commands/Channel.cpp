#include "../includes/Channel.hpp"
#include "../includes/Client.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/socket.h> // Add this for send() function
#include <unistd.h>     // Add this for close() function

Channel::Channel(const std::string& name) 
    : _name(name), _topic(""), _key(""), _inviteOnly(false), 
      _topicRestricted(false), _userLimit(0) {
    std::cout << "New channel created: " << name << std::endl;
}

Channel::~Channel() {
    std::cout << "Channel destroyed: " << _name << std::endl;
}

const std::string& Channel::getName() const {
    return _name;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
} 

void Channel::addClient(Client* client) {
    _clients[client] = false;
}

void Channel::removeClient(Client* client) {
    _clients.erase(client);
    removeInvited(client);
}

bool Channel::hasClient(Client* client) const {
    return _clients.find(client) != _clients.end();
}

std::vector<Client*> Channel::getClients() const {
    std::vector<Client*> clients;
    for (std::map<Client*, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        clients.push_back(it->first);
    }
    return clients;
}

size_t Channel::getClientCount() const {
    return _clients.size();
}

void Channel::setOperator(Client* client, bool isOp) {
    if (_clients.find(client) != _clients.end()) {
        _clients[client] = isOp;
    }
}

bool Channel::isOperator(Client* client) const {
    std::map<Client*, bool>::const_iterator it = _clients.find(client);
    if (it != _clients.end()) {
        return it->second;
    }
    return false;
}

void Channel::setKey(const std::string& key) {
    _key = key;
}

const std::string& Channel::getKey() const {
    return _key;
}

void Channel::setInviteOnly(bool inviteOnly) {
    _inviteOnly = inviteOnly;
}

bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setTopicRestricted(bool restricted) {
    _topicRestricted = restricted;
}

bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
}

int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::addInvited(Client* client) {
    _invited.push_back(client);
}

void Channel::removeInvited(Client* client) {
    _invited.erase(std::remove(_invited.begin(), _invited.end(), client), _invited.end());
}

bool Channel::isInvited(Client* client) const {
    return std::find(_invited.begin(), _invited.end(), client) != _invited.end();
}

void Channel::broadcast(const std::string& message, Client* exclude) {
    for (std::map<Client*, bool>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->first != exclude) {
            ::send(it->first->getFd(), message.c_str(), message.length(), 0);
        }
    }
}

void Channel::sendNamesList(Client* client) {
    std::stringstream names;
    names << ":irc.server 353 " << client->getNickname() << " = " << _name << " :";
    
    bool first = true;
    for (std::map<Client*, bool>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (!first) names << " ";
        if (it->second) names << "@";
        names << it->first->getNickname();
        first = false;
    }
    names << "\r\n";
    
    ::send(client->getFd(), names.str().c_str(), names.str().length(), 0);
    
    // FIXED: Use string concatenation with + instead of <<
    std::string endNames = ":irc.server 366 " + client->getNickname() + " " + _name + " :End of /NAMES list\r\n";
    ::send(client->getFd(), endNames.c_str(), endNames.length(), 0);
}

void Channel::sendTopic(Client* client) {
    if (_topic.empty()) {
        // FIXED: Use string concatenation with + instead of <<
        std::string response = ":irc.server 331 " + client->getNickname() + " " + _name + " :No topic is set\r\n";
        ::send(client->getFd(), response.c_str(), response.length(), 0);
    } else {
        // FIXED: Use string concatenation with + instead of <<
        std::string response = ":irc.server 332 " + client->getNickname() + " " + _name + " :" + _topic + "\r\n";
        ::send(client->getFd(), response.c_str(), response.length(), 0);
    }
}