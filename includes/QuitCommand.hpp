#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include "BaseCommand.hpp"

class QuitCommand : public BaseCommand {
public:
    QuitCommand(Server* server);
    ~QuitCommand();

    void execute(Client* client, const std::vector<std::string>& args);
};

#endif