#include "../includes/Client.hpp"
#include <iostream>
#include <sstream>

Client::Client(int fd) 
    : _fd(fd), _nickname(""), _username(""), _realname(""), _hostname("localhost"), 
      _authenticated(false), _registered(false) {
    std::cout << "New client connected with FD: " << fd << std::endl;
}

Client::~Client() {
    std::cout << "Client destroyed: " << _nickname << " (FD: " << _fd << ")" << std::endl;
}

int Client::getFd() const {
    return _fd;
}

const std::string& Client::getNickname() const {
    return _nickname;
}

const std::string& Client::getUsername() const {
    return _username;
}

const std::string& Client::getRealname() const {
    return _realname;
}

const std::string& Client::getHostname() const {
    return _hostname;
}

void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}

void Client::setRealname(const std::string& realname) {
    _realname = realname;
}

void Client::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void Client::setAuthenticated(bool auth) {
    _authenticated = auth;
}

void Client::setRegistered(bool reg) {
    _registered = reg;
}

bool Client::isAuthenticated() const {
    return _authenticated;
}

bool Client::isRegistered() const {
    return _registered;
}

void Client::appendToBuffer(const std::string& data) {
    _buffer += data;
}

std::string Client::getBuffer() const {
    return _buffer;
}

void Client::clearBuffer() {
    _buffer.clear();
}

bool Client::hasCompleteCommand() const {
    return _buffer.find("\n") != std::string::npos;
}

std::string Client::extractCommand() {
    size_t pos = _buffer.find("\n");
    if (pos == std::string::npos) {
        return "";
    }
    
    std::string command = _buffer.substr(0, pos);
    // Remove \r if present
    if (!command.empty() && command[command.length()-1] == '\r') {
        command = command.substr(0, command.length()-1);
    }
    _buffer.erase(0, pos + 1);
    
    return command;
}

std::string Client::getPrefix() const {
    if (_nickname.empty()) {
        return "";
    }
    
    std::string prefix = _nickname;
    if (!_username.empty()) {
        prefix += "!" + _username;
    }
    if (!_hostname.empty()) {
        prefix += "@" + _hostname;
    }
    
    return prefix;
}

std::string Client::getUserMask() const {
    return _nickname + "!" + _username + "@" + _hostname;
}