#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "BaseCommand.hpp"

class TopicCommand : public BaseCommand {
public:
    TopicCommand(Server* server);
    ~TopicCommand();

    void execute(Client* client, const std::vector<std::string>& args);
};

#endif