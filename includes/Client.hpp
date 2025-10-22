#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    std::string _buffer;
    bool _authenticated;
    bool _registered;
    
public:
    Client(int fd);
    ~Client();
    
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    
    void setAuthenticated(bool auth);
    void setRegistered(bool reg);
    bool isAuthenticated() const;
    bool isRegistered() const;
    
    void appendToBuffer(const std::string& data);
    std::string getBuffer() const;
    void clearBuffer();
    bool hasCompleteCommand() const;
    std::string extractCommand();
    
    std::string getPrefix() const;
};

#endif