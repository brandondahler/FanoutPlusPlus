#include "config.h"

#include "NotificationChannel.h"

#include "NotificationClientHandler.h"

using namespace std;


/// Static variables
map<string, NotificationChannel*> NotificationChannel::channelMap;


/// Static functions

void NotificationChannel::CleanupEmptyChannels()
{
    // Log debug message
    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_INFO, "NotificationChannel", "Cleaning up empty channels");

    // List to record what channels to delete
    list<string> deleteChannels;

    for (map<string, NotificationChannel*>::iterator it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        // If no more clients are in the channel
        if (it->second->clients.size() == 0)
        {
            // Log debug message
            ostringstream logMessage;
            logMessage << "Cleaning up channel " << it->first;
            FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_DEBUG, "NotificationChannel", "Cleaning up empty channels");

            // Add channel to delete list, delete channel object
            deleteChannels.push_back(it->first);
            delete it->second;
        }
    }

    // Remove each channel that has been deleted from the channel map
    for (list<string>::iterator it = deleteChannels.begin(); it != deleteChannels.end(); ++it)
        channelMap.erase(*it);

}

void NotificationChannel::CleanupChannels()
{
    // Log debug message
    ostringstream logMessage;
    logMessage << "Cleaning up " << channelMap.size() << " channels";
    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_DEBUG, "NotificationChannel", logMessage);


    // Delete all NotificationChannels
    for (map<string, NotificationChannel*>::iterator it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second)
            delete it->second;
    }

    // Remove them from the map
    channelMap.clear();
}

void NotificationChannel::UnsubscribeFromAll(NotificationClientHandler* client)
{
    // Log debug message
    ostringstream logMessage;
    logMessage << "Unsubscribing client " << client->GetClientId() << " from all channels";
    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_DEBUG, "NotificationChannel", logMessage);


    // Remove client from all channels in the map
    for (map<string, NotificationChannel*>::iterator it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second)
            it->second->RemoveClient(client);
    }

}


void NotificationChannel::SubscribeToChannel(NotificationClientHandler* client, string channel)
{
    // Get channel, create if it doesn't exist
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
    {
        nc = new NotificationChannel(channel);
        channelMap[channel] = nc;
    }

    // Add client to retrieved channel
    nc->AddClient(client);
}

void NotificationChannel::UnsubscribeFromChannel(NotificationClientHandler* client, string channel)
{
    // Get channel, ignore if channel doesn't exist
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
        return;

    // Remove client from retrieved channel
    nc->RemoveClient(client);
}

void NotificationChannel::AnnounceToChannel(NotificationClientHandler* client, string channel, string announcement)
{
    // Get channel, ignore if channel doesn't exist
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
        return;

    // Announce message to channel from client
    nc->Announce(client, announcement);
}


/// Private functions

void NotificationChannel::AddClient(NotificationClientHandler* client)
{
    // Log debug message
    ostringstream logMessage;
    logMessage << "Channel "  << channelName << " adding client " << client->GetClientId();
    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_DEBUG, "NotificationChannel", logMessage);

    // Add client to client list
    clients.insert(client);
}

void NotificationChannel::RemoveClient(NotificationClientHandler* client)
{
    // Log debug message
    ostringstream logMessage;
    logMessage << "Channel "  << channelName << " removing client " << client->GetClientId();
    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_DEBUG, "NotificationChannel", logMessage);

    // Remove client from channel
    clients.erase(client);
}

void NotificationChannel::Announce(NotificationClientHandler* client, string announcement)
{
    // Don't announce unless we're in the channel, sneaky hacker
    if (clients.find(client) == clients.end())
        return;

    // Log debug message
    ostringstream logMessage;
    logMessage << "Channel "  << channelName << " announcing message " << announcement;
    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_DEBUG, "NotificationChannel", logMessage);


    // Announce to everyone in the channel except the sender
    ostringstream ossAnnouncement;
    ossAnnouncement << channelName << "!" << announcement << "\n";
    string announcementString = ossAnnouncement.str();

    // Massage string for send data
    const char* announcementConstChar = announcementString.c_str();
    size_t announcementStringSize = announcementString.size();

    // Loop through the clients that are in the channel
    for (set<NotificationClientHandler*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        // If the client isn't the sender, send the message
        if ((*it) != client)
            (*it)->SendData(announcementConstChar, announcementStringSize);
    }
}

