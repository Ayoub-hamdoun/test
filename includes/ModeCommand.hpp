#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "BaseCommand.hpp"

class ModeCommand : public BaseCommand {
public:
    ModeCommand(Server* server);
    ~ModeCommand();

    void execute(Client* client, const std::vector<std::string>& args);

private:
    void handleChannelMode(Client* client, const std::vector<std::string>& args);
};

#endif