#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include "BaseCommand.hpp"

class NickCommand : public BaseCommand {
public:
    NickCommand(Server* server);
    ~NickCommand();

    void execute(Client* client, const std::vector<std::string>& args);

private:
    bool isValidNickname(const std::string& nick);
};

#endif