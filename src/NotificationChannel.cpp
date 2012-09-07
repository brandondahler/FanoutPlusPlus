#include "config.h"

#include "NotificationChannel.h"

#include "NotificationClientHandler.h"

using namespace std;

void NotificationChannel::AddClient(NotificationClientHandler* client)
{
    clients.insert(client);
}

void NotificationChannel::RemoveClient(NotificationClientHandler* client)
{
    clients.erase(client);
}

void NotificationChannel::Announce(NotificationClientHandler* client, string announceHash)
{
    // Don't announce unless we're in the channel
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
