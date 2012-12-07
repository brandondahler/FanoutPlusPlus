#ifndef NOTIFICATIONCHANNEL_H
#define NOTIFICATIONCHANNEL_H

#include <iostream>
#include <string>
#include <set>
#include <map>

class NotificationClientHandler;

class NotificationChannel
{
    public:
        static void CleanupChannels();
        static void UnsubscribeFromAll(NotificationClientHandler* client);

        static void SubscribeToChannel(NotificationClientHandler* client, std::string channel);
        static void UnsubscribeFromChannel(NotificationClientHandler* client, std::string channel);
        static void AnnounceToChannel(NotificationClientHandler* client, std::string channel, std::string announceHash);

    private:
        NotificationChannel(std::string channel) : channelName(channel) { };

        void AddClient(NotificationClientHandler* client);
        void RemoveClient(NotificationClientHandler* client);
        void Announce(NotificationClientHandler* client, std::string announceHash);

        std::set<NotificationClientHandler*> clients;
        std::string channelName;

        static std::map<std::string, NotificationChannel*> channelMap;
};

#endif // NOTIFICATIONCHANNEL_H
