#ifndef NOTIFICATIONCLIENTHANDLER_H
#define NOTIFICATIONCLIENTHANDLER_H


#include <pthread.h>

typedef int socket_t;

class NotificationClientHandler
{
    public:
        NotificationClientHandler(socket_t cSocket);
        ~NotificationClientHandler();

        void SendData(const void* data, unsigned int length);

    private:
        pthread_t clientThread;
        socket_t clientSocket;
        pthread_mutex_t socketMutex;

        static void* ProcessDataThread(void* param);
        void ProcessData();

};

#endif // NOTIFICATIONCLIENTHANDLER_H
