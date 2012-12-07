#include "config.h"

#include "NotificationServer.h"

#include "FanoutLogger.h"
#include "NotificationClientHandler.h"
#include "NotificationChannel.h"

#include <string>

#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif

#ifdef HAVE_EVENT2_EVENT_H
    #include <event2/event.h>
#endif

#ifdef HAVE_NETINET_IN_H
    #include <netinet/in.h>
#endif
#ifdef HAVE_NETINET_TCP_H
    #include <netinet/tcp.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
    #include <sys/socket.h>
#endif
#ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
#endif
#ifdef HAVE_SYS_UN_H
    #include <sys/un.h>
#endif

#ifdef HAVE_UNISTD_H
    #include <unistd.h>
#endif

using namespace std;

namespace NotificationServer
{

    int listenSocket = -1;
    event_base* eventBase = NULL;

    void StartServer(unsigned short port)
    {
        if (eventBase || listenSocket != -1)
            ShutdownServer();

        eventBase = event_base_new();
        if (!eventBase)
        {
            FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "NotificationServer", "Error starting new event base.");
            return;
        }

        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        evutil_make_socket_nonblocking(listenSocket);
        evutil_make_listen_socket_reuseable(listenSocket);

        if (listenSocket < 0)
        {
            FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "NotificationServer", "Error opening listen socket.");
            return;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);
        memset(&serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));


        if (bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
        {
            ostringstream errorMessage;
            errorMessage << errno << " - " << "Error binding to the listening socket.";
            FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "NotificationServer", errorMessage);
            return;
        }

        if (listen(listenSocket, 5) < 0)
        {
            ostringstream errorMessage;
            errorMessage << errno << " - " << "Listening to the listening socket.";
            FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "NotificationServer", errorMessage);
            return;
        }


        event* listener_event = event_new(eventBase, listenSocket, EV_READ|EV_PERSIST, &NotificationClientHandler::AcceptClient, (void*) eventBase);
        event_add(listener_event, NULL);

        event_base_dispatch(eventBase);
    }

    void WaitForServerShutdown()
    {
        ShutdownServer();
    }


    void ShutdownServer()
    {
        if (eventBase)
            event_base_loopbreak(eventBase);

        NotificationClientHandler::CleanupClients();
        NotificationChannel::CleanupChannels();

        if (listenSocket != -1)
        {
            close(listenSocket);
            listenSocket = -1;
        }

        if (eventBase)
        {
            event_base_free(eventBase);
            eventBase = NULL;
        }

    }
}
