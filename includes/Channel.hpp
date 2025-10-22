#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>

class Client;

class Channel {
private:
    std::string _name;
    std::string _topic;
    std::map<Client*, bool> _clients;
    std::string _key;
    bool _inviteOnly;
    bool _topicRestricted;
    int _userLimit;
    std::vector<Client*> _invited;
    
public:
    Channel(const std::string& name);
    ~Channel();
    
    const std::string& getName() const;
    const std::string& getTopic() const;
    void setTopic(const std::string& topic);
    
    void addClient(Client* client);
    void removeClient(Client* client);
    bool hasClient(Client* client) const;
    std::vector<Client*> getClients() const;
    size_t getClientCount() const;
    
    void setOperator(Client* client, bool isOp);
    bool isOperator(Client* client) const;
    
    void setKey(const std::string& key);
    const std::string& getKey() const;
    
    void setInviteOnly(bool inviteOnly);
    bool isInviteOnly() const;
    
    void setTopicRestricted(bool restricted);
    bool isTopicRestricted() const;
    
    void setUserLimit(int limit);
    int getUserLimit() const;
    
    void addInvited(Client* client);
    void removeInvited(Client* client);
    bool isInvited(Client* client) const;
    
    void broadcast(const std::string& message, Client* exclude = NULL);
    void sendNamesList(Client* client);
    void sendTopic(Client* client);
};

#endif