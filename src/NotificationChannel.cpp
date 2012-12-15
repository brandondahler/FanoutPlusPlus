#include "config.h"

#include "NotificationChannel.h"

#include "NotificationClientHandler.h"

using namespace std;


/// Static variables
map<string, NotificationChannel*> NotificationChannel::channelMap;


/// Static functions

void NotificationChannel::CleanupChannels()
{
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
    // Remove client from all channels in the map
    for (map<string, NotificationChannel*>::iterator it = channelMap.begin(); it != channelMap.end(); ++it)
        it->second->RemoveClient(client);
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

void NotificationChannel::AnnounceToChannel(NotificationClientHandler* client, string channel, string announceHash)
{
    // Get channel, ignore if channel doesn't exist
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
        return;

    // Announce hash to channel from client
    nc->Announce(client, announceHash);
}


/// Private functions

void NotificationChannel::AddClient(NotificationClientHandler* client)
{
    // Add client to client list
    clients.insert(client);
}

void NotificationChannel::RemoveClient(NotificationClientHandler* client)
{
    // Remove client from channel
    clients.erase(client);

    // If no more clients are in the channel, remove channel from the list and delete self
    if (clients.size() == 0)
    {
        channelMap.erase(channelName);
        delete this;
    }
}

void NotificationChannel::Announce(NotificationClientHandler* client, string announceHash)
{
    // Don't announce unless we're in the channel, sneaky hacker
    if (clients.find(client) == clients.end())
        return;

    // Announce to everyone in the channel except the sender
    string announceMessage = string(channelName + "!" + announceHash + "\n");

    // Loop through the clients that are in the channel
    for (set<NotificationClientHandler*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        // If the client isn't the sender, send the message
        if ((*it) != client)
            (*it)->SendData(announceMessage.c_str(), announceMessage.size());
    }
}

