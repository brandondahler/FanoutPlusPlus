#include "config.h"

#include "NotificationClientCommands.h"

#include "NotificationChannel.h"
#include "NotificationClientHandler.h"


using namespace std;


// Define static variable initialization
map<string, CommandFunction> NotificationClientCommands::commandMap;
bool NotificationClientCommands::commandMapCreated = false;


// Define commands and their respective function pointers
const NotificationClientCommands::ClientCommand NotificationClientCommands::COMMANDS[] = {
    { "ping",           &NotificationClientCommands::RespondToPing  },
    { "subscribe",      &NotificationClientCommands::SubscribeToChannel },
    { "unsubscribe",    &NotificationClientCommands::UnsubscribeFromChannel },
    { "announce",       &NotificationClientCommands::AnnounceToChannel }
};


/// Public methods

NotificationClientCommands::NotificationClientCommands()
{
    // Check if commandMap is already created
    if (!commandMapCreated)
    {
        // Create it and set commandMap as being created
        CreateCommandMap();
        commandMapCreated = true;
    }

}

bool NotificationClientCommands::HandleCommand(NotificationClientHandler& handler, std::string commandName, std::istringstream& commandData)
{
    // Get command function from map
    CommandFunction cf = commandMap[commandName];
    if (cf)
    {
        // Call command function if defined
        cf(handler, commandData);
        return true;
    }

    // Invalid command given
    return false;
}


/// Protected methods

void NotificationClientCommands::RespondToPing(NotificationClientHandler& handler, istringstream& commandData)
{
    // Log pong message
    handler.LogMessage("Pong");

    // Write out time to string
    ostringstream timeStream;
    timeStream << time(NULL) << "000" << '\n';

    // Convert to string, send out to client
    string timeString = timeStream.str();
    handler.SendData(timeString.c_str(), timeString.length());
}

void NotificationClientCommands::SubscribeToChannel(NotificationClientHandler& handler, istringstream& commandData)
{
    // Read in channel
    string channel;
    commandData >> channel;

    // Log subscribing message
    ostringstream logMessage;
    logMessage << "Subscribing to " << channel;
    handler.LogMessage(logMessage.str());

    // Subscribe to desired channel
    NotificationChannel::SubscribeToChannel(&handler, channel);
}

void NotificationClientCommands::UnsubscribeFromChannel(NotificationClientHandler& handler, istringstream& commandData)
{
    // Read in channel
    string channel;
    commandData >> channel;

    // Log unsubscribing message
    ostringstream logMessage;
    logMessage << "Unsubscribing from " << channel;
    handler.LogMessage(logMessage.str());

    // Unsubscribe from desired channel
    NotificationChannel::UnsubscribeFromChannel(&handler, channel);
}

void NotificationClientCommands::AnnounceToChannel(NotificationClientHandler& handler, istringstream& commandData)
{
    string channel;
    string announceData;

    // Read in channel and announce data
    commandData >> channel;
    commandData.ignore();
    getline(commandData, announceData);

    // Log announcing message
    ostringstream logMessage;
    logMessage << "Announcing to " << channel << " the message <" << announceData << ">";
    handler.LogMessage(logMessage.str());

    // Announce desired message to desired channel
    NotificationChannel::AnnounceToChannel(&handler, channel, announceData);
}


/// Private static methods

void NotificationClientCommands::CreateCommandMap()
{
    // Loop through commands and create map of commandFunctions
    for (unsigned int x = 0; x < (sizeof(COMMANDS) / sizeof(COMMANDS[0])) ; ++x)
        commandMap[string(COMMANDS[x].command)] = COMMANDS[x].commandFunction;
}

