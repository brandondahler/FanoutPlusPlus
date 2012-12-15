#ifndef NOTIFICATIONCLIENTCOMMANDS_H
#define NOTIFICATIONCLIENTCOMMANDS_H

#include <map>
#include <sstream>
#include <string>

class NotificationClientHandler;
typedef void (* CommandFunction)(NotificationClientHandler&, std::istringstream&);


class NotificationClientCommands
{
    public:
        NotificationClientCommands();
        bool HandleCommand(NotificationClientHandler& handler, std::string commandName, std::istringstream& commandData);

    protected:
        static std::map<std::string, CommandFunction> commandMap;

        static void RespondToPing(NotificationClientHandler& handler, std::istringstream& commandData);
        static void SubscribeToChannel(NotificationClientHandler& handler, std::istringstream& commandData);
        static void UnsubscribeFromChannel(NotificationClientHandler& handler, std::istringstream& commandData);
        static void AnnounceToChannel(NotificationClientHandler& handler, std::istringstream& commandData);

    private:
        struct ClientCommand
        {
            const char* command;
            CommandFunction commandFunction;
        };

        static const ClientCommand COMMANDS[];
        static bool commandMapCreated;

        static void CreateCommandMap();


};

#endif // NOTIFICATIONCLIENTCOMMANDS_H
