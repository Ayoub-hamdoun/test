#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "BaseCommand.hpp"

class TopicCommand : public BaseCommand {
public:
    TopicCommand(Server* server);
    virtual ~TopicCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif