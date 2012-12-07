#include "config.h"

#include "NotificationChannel.h"

#include "NotificationClientHandler.h"

using namespace std;

// Static variables
map<string, NotificationChannel*> NotificationChannel::channelMap;

// Static functions

void NotificationChannel::CleanupChannels()
{
    for (map<string, NotificationChannel*>::iterator it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second)
            delete it->second;
    }
    channelMap.clear();
}

void NotificationChannel::UnsubscribeFromAll(NotificationClientHandler* client)
{
    for (map<string, NotificationChannel*>::iterator it = channelMap.begin(); it != channelMap.end(); ++it)
        it->second->RemoveClient(client);
}


void NotificationChannel::SubscribeToChannel(NotificationClientHandler* client, string channel)
{
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
    {
        nc = new NotificationChannel(channel);
        channelMap[channel] = nc;
    }

    nc->AddClient(client);
}

void NotificationChannel::UnsubscribeFromChannel(NotificationClientHandler* client, string channel)
{
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
        return;

    nc->RemoveClient(client);
}

void NotificationChannel::AnnounceToChannel(NotificationClientHandler* client, string channel, string announceHash)
{
    NotificationChannel* nc = channelMap[channel];
    if (!nc)
        return;

    nc->Announce(client, announceHash);
}

// Private functions


void NotificationChannel::AddClient(NotificationClientHandler* client)
{
    clients.insert(client);
}

void NotificationChannel::RemoveClient(NotificationClientHandler* client)
{
    clients.erase(client);

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

    for (set<NotificationClientHandler*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it) != client)
            (*it)->SendData(announceMessage.c_str(), announceMessage.size());
    }
}

