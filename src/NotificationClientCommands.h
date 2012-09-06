#ifndef NOTIFICATIONCLIENTCOMMANDS_H
#define NOTIFICATIONCLIENTCOMMANDS_H

#include <string>
#include <sstream>

class NotificationClientHandler;
typedef void (* CommandFunction)(NotificationClientHandler&, std::istringstream&);


class NotificationClientCommands
{
    public:
        NotificationClientCommands();
        bool HandleCommand(NotificationClientHandler& handler, std::string commandName, std::istringstream& commandData);

    protected:
        struct ClientCommand
        {
            std::string command;
            CommandFunction commandFunction;
        };

        static const ClientCommand COMMANDS[];

        static void RespondToPing(NotificationClientHandler& handler, std::istringstream& commandData);
        static void SubscribeToChannel(NotificationClientHandler& handler, std::istringstream& commandData);
        static void UnsubscribeFromChannel(NotificationClientHandler& handler, std::istringstream& commandData);
        static void AnnounceToChannel(NotificationClientHandler& handler, std::istringstream& commandData);

};

#endif // NOTIFICATIONCLIENTCOMMANDS_H
