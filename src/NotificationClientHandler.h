#ifndef NOTIFICATIONCLIENTHANDLER_H
#define NOTIFICATIONCLIENTHANDLER_H

#include "NotificationClientCommands.h"

#include <pthread.h>
#include <string>

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

typedef int socket_t;

class NotificationClientHandler
{
    public:
        NotificationClientHandler(socket_t cSocket);
        ~NotificationClientHandler();

        void SendData(const void* data, unsigned int length);
        void LogMessage(std::string message);

    private:

        uint64_t clientId;

        pthread_t clientThread;
        socket_t clientSocket;
        pthread_mutex_t socketMutex;

        static void* ProcessDataThread(void* param);
        void ProcessData();


        static const int COMMAND_MAX_LENGTH;

        static NotificationClientCommands commands;

        static pthread_mutex_t clientIdMutex;
        static uint64_t nextClientId;

};

#endif // NOTIFICATIONCLIENTHANDLER_H
