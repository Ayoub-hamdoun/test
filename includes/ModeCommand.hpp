#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "BaseCommand.hpp"

class ModeCommand : public BaseCommand {
private:
    void handleChannelMode(Client* client, const std::vector<std::string>& args);
    
public:
    ModeCommand(Server* server);
    virtual ~ModeCommand();
    virtual void execute(Client* client, const std::vector<std::string>& args);
};

#endif