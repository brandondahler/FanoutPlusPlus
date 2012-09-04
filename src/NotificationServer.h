#ifndef NOTIFICATIONSERVER_H
#define NOTIFICATIONSERVER_H

#include "NotificationChannel.h"

#include <iostream>
#include <map>

namespace NotificationServer
{
    void StartServer(unsigned short port);
    void ShutdownServer();

    void SubscribeToChannel(NotificationClientHandler* client, std::string channel);
    void UnsubscribeFromChannel(NotificationClientHandler* client, std::string channel);
    void AnnounceToChannel(NotificationClientHandler* client, std::string channel, std::string announceHash);
    void ClientClose(NotificationClientHandler* client);

}

#endif // NOTIFICATIONSERVER_H
