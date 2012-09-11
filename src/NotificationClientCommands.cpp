#include "config.h"

#include "NotificationClientCommands.h"

#include "NotificationServer.h"
#include "NotificationClientHandler.h"


using namespace std;

const NotificationClientCommands::ClientCommand NotificationClientCommands::COMMANDS[] = {
    { string("ping"), &NotificationClientCommands::RespondToPing },
    { string("subscribe"), &NotificationClientCommands::SubscribeToChannel },
    { string("unsubscribe"), &NotificationClientCommands::SubscribeToChannel },
    { string("announce"), &NotificationClientCommands::SubscribeToChannel },

};

NotificationClientCommands::NotificationClientCommands()
{

}

bool NotificationClientCommands::HandleCommand(NotificationClientHandler& handler, std::string commandName, std::istringstream& commandData)
{
    for (unsigned int x = 0; x < sizeof(COMMANDS); ++x)
    {
        if (COMMANDS[x].command == commandName)
        {
            COMMANDS[x].commandFunction(handler, commandData);
            return true;
        }
    }

    return false;
}

/*
 *  Private functions
 */

void NotificationClientCommands::RespondToPing(NotificationClientHandler& handler, istringstream& commandData)
{
    handler.LogMessage("Pong");

    ostringstream timeStream;
    timeStream << time(NULL) << "000" << '\n';

    string timeString = timeStream.str();
    handler.SendData(timeString.c_str(), timeString.length());
}

void NotificationClientCommands::SubscribeToChannel(NotificationClientHandler& handler, istringstream& commandData)
{
    string channel;
    commandData >> channel;

    handler.LogMessage(string("Subscribing to ") + channel);
    NotificationServer::SubscribeToChannel(&handler, channel);
}

void NotificationClientCommands::UnsubscribeFromChannel(NotificationClientHandler& handler, istringstream& commandData)
{
    string channel;
    commandData >> channel;

    handler.LogMessage(string("Unsubscribing from ") + channel);
    NotificationServer::UnsubscribeFromChannel(&handler, channel);
}

void NotificationClientCommands::AnnounceToChannel(NotificationClientHandler& handler, istringstream& commandData)
{
    string channel;
    string announceData;

    commandData >> channel;
    getline(commandData, announceData);

    handler.LogMessage(string("Announcing to ") + channel + " data " + announceData);
    NotificationServer::AnnounceToChannel(&handler, channel, announceData);
}

