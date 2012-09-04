#ifndef NOTIFICATIONCHANNEL_H
#define NOTIFICATIONCHANNEL_H

#include <iostream>
#include <set>

class NotificationClientHandler;

class NotificationChannel
{
    public:
        NotificationChannel(std::string channel) : channelName(channel) { };

        size_t ClientCount() { return clients.size(); }

        void AddClient(NotificationClientHandler* client);
        void RemoveClient(NotificationClientHandler* client);
        void Announce(NotificationClientHandler* client, std::string announceHash);

    private:
        std::set<NotificationClientHandler*> clients;
        std::string channelName;
};

#endif // NOTIFICATIONCHANNEL_H
