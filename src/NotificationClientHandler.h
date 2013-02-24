#ifndef NOTIFICATIONCLIENTHANDLER_H
#define NOTIFICATIONCLIENTHANDLER_H

#include "NotificationClientCommands.h"
#include "FanoutLogger.h"

#include <string>
#include <sstream>
#include <list>

#ifdef HAVE_EVENT2_UTIL_H
    #include <event2/util.h>
#endif

#ifdef HAVE_STDINT_H
    #include <stdint.h>
#endif

struct event;
struct event_base;

class NotificationClientHandler
{
    public:

        ~NotificationClientHandler();

        uint64_t GetClientId() { return clientId; }

        void SendData(const void* data, int length);
        void LogMessage(std::string message, FanoutLogger::MessageSeverity severity = FanoutLogger::FANOUT_LOG_INFO);

        static void AcceptClient(evutil_socket_t listeningSocket, short flags, void* socketParam);
        static void CleanupClients();

    private:
        uint64_t clientId;

        int clientSocket;
        event_base* eventBase;
        event* processEvent;

        std::ostringstream lastData;

	NotificationClientCommands commands;


        NotificationClientHandler(int cSocket, event_base* eventBase);

        static void ProcessData(evutil_socket_t clientSocket, short flags, void* processParam);
        void ProcessData();

        static void SendData(evutil_socket_t clientSocket, short flags, void* sendParam);

        static const int COMMAND_MAX_LENGTH;

        static std::list<NotificationClientHandler*> clientList;
        static uint64_t nextClientId;

};

#endif // NOTIFICATIONCLIENTHANDLER_H
