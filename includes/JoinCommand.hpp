#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "BaseCommand.hpp"

class JoinCommand : public BaseCommand {
    private:
        std::vector<std::string> splitByComma(const std::string& str);
        void joinSingleChannel(Client* client, const std::string& channelName, const std::string& channelKey);

    public:
        JoinCommand(Server* server);
        ~JoinCommand();

        void execute(Client* client, const std::vector<std::string>& args);
};

#endif