#include "NotificationServer.h"
#include "NotificationClientHandler.h"

#include <iostream>
#include <cstring>
#include <list>

#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>

#ifdef WIN32
    #include <winsock2.h>

    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <errno.h>

    #define INVALID_SOCKET -1
#endif

typedef int socket_t;


using namespace std;

namespace NotificationServer
{

    pthread_t acceptThread;
    pthread_mutex_t serverChannelsMutex;
    pthread_mutex_t clientListMutex;

    socket_t listenSocket = INVALID_SOCKET;

    list<NotificationClientHandler*> clientList;
    map<string, NotificationChannel*> serverChannels;


    void* AcceptClients(void* param);


    void StartServer(unsigned short port)
    {
        #ifdef WIN32
            WSADATA wsaData;

            if (WSAStartup( MAKEWORD(2,2), &wsaData) != NO_ERROR)
               throw "Error at WSAStartup()";
        #endif

        pthread_mutex_init(&serverChannelsMutex, 0);
        pthread_mutex_init(&clientListMutex, 0);


        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        cout << errno << endl;


        if (listenSocket < 0)
            throw "ERROR opening socket";

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);
        memset(&serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));


        int bindError = bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
        if (bindError)
        {
            cout << errno << endl;
            throw "ERROR on binding";
        }



        listen(listenSocket, 5);

        if (pthread_create(&acceptThread, NULL, &AcceptClients, NULL) != 0)
            throw "Error creating the accept thread.";

    }


    void ShutdownServer()
    {
        pthread_mutex_lock(&clientListMutex);
        for (list<NotificationClientHandler*>::iterator it = clientList.begin(); it != clientList.end(); ++it)
        {
            if (*it)
                delete (*it);
        }
        clientList.clear();
        pthread_mutex_unlock(&clientListMutex);


        pthread_mutex_lock(&serverChannelsMutex);
        for (map<string, NotificationChannel*>::iterator it = serverChannels.begin(); it != serverChannels.end(); ++it)
        {
            if (it->second)
                delete it->second;
        }
        serverChannels.clear();
        pthread_mutex_unlock(&serverChannelsMutex);
    }

    void SubscribeToChannel(NotificationClientHandler* client, string channel)
    {
        pthread_mutex_lock(&serverChannelsMutex);

        NotificationChannel* nc = serverChannels[channel];
        if (!nc)
        {
            nc = new NotificationChannel(channel);
            serverChannels[channel] = nc;
        }

        nc->AddClient(client);

        pthread_mutex_unlock(&serverChannelsMutex);

        pthread_mutex_destroy(&serverChannelsMutex);
        pthread_mutex_destroy(&clientListMutex);

    }

    void UnsubscribeFromChannel(NotificationClientHandler* client, string channel)
    {
        pthread_mutex_lock(&serverChannelsMutex);

        NotificationChannel* nc = serverChannels[channel];
        if (!nc)
        {
            pthread_mutex_unlock(&serverChannelsMutex);
            return;
        }

        nc->RemoveClient(client);

        if (nc->ClientCount() == 0)
        {
            serverChannels.erase(channel);
            delete nc;
        }

        pthread_mutex_unlock(&serverChannelsMutex);
    }

    void AnnounceToChannel(NotificationClientHandler* client, string channel, string announceHash)
    {
        pthread_mutex_lock(&serverChannelsMutex);

        NotificationChannel* nc = serverChannels[channel];
        if (!nc)
        {
            pthread_mutex_unlock(&serverChannelsMutex);
            return;
        }

        nc->Announce(client, announceHash);

        pthread_mutex_unlock(&serverChannelsMutex);
    }

    void ClientClose(NotificationClientHandler* client)
    {
        pthread_mutex_lock(&serverChannelsMutex);

        for (map<string, NotificationChannel*>::iterator it = serverChannels.begin(); it != serverChannels.end(); ++it)
            it->second->RemoveClient(client);

        pthread_mutex_unlock(&serverChannelsMutex);

        pthread_mutex_lock(&clientListMutex);
        clientList.remove(client);
        pthread_mutex_unlock(&clientListMutex);

        delete client;
    }


    void* AcceptClients(void* param)
    {
        while (true)
        {
            sockaddr_in clientAddress;
            memset(&clientAddress, 0, sizeof(clientAddress));

            socklen_t clientAddressSize = sizeof(clientAddress);

            socket_t clientSocket = accept(listenSocket, (struct sockaddr *) &clientAddress, &clientAddressSize);
            if (clientSocket < 0)
                throw "ERROR on accept";

            NotificationClientHandler* client = new NotificationClientHandler(clientSocket);

            pthread_mutex_lock(&clientListMutex);
            clientList.push_back(client);
            pthread_mutex_unlock(&clientListMutex);
        }
    }

}
