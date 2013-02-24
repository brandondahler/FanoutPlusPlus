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
    #ifdef USE_IPV6
    int listenSocketV6 = -1;
    #endif

    event_base* eventBase = NULL;
    const struct timeval cleanupTimeout = {300, 0};

    void CleanupEmptyChannelsCallback(evutil_socket_t nullSocket, short flags, void* socketParam);

    void StartServer(unsigned short port)
    {
        // Cleanup in case the server was started before
        if (eventBase || listenSocket != -1)
            ShutdownServer();

        // Clean up if IPv6 is used and the socket is still valid
        #ifdef USE_IPV6
            if (listenSocketV6 != -1)
                ShutdownServer();
        #endif

        // Create new event base
        eventBase = event_base_new();
        if (!eventBase)
        {
            FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", "Error starting new event base.");
            return;
        }

        // Bind for IPv4
        {
            // Create new listen socket, make it non-blocking and reusable
            listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

            evutil_make_socket_nonblocking(listenSocket);
            evutil_make_listen_socket_reuseable(listenSocket);

            if (listenSocket < 0)
            {
                FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", "Error opening IPv4 listen socket.");
                return;
            }

            // Listen on all IPv4 addresses, port as passed in
            sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = INADDR_ANY;
            serverAddress.sin_port = htons(port);
            memset(&serverAddress.sin_zero, 0, sizeof(serverAddress.sin_zero));

            // Bind to socket
            if (bind(listenSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
            {
                ostringstream errorMessage;
                errorMessage << errno << " - " << "Error binding to the IPv4 listening socket.";
                FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", errorMessage);
                return;
            }

            // Listen on socket
            if (listen(listenSocket, 5) < 0)
            {
                ostringstream errorMessage;
                errorMessage << errno << " - " << "Listening to the IPv4 listening socket.";
                FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", errorMessage);
                return;
            }

            // Create new event to accept clients, add it
            event* listener_event = event_new(eventBase, listenSocket, EV_READ|EV_PERSIST, &NotificationClientHandler::AcceptClient, (void*) eventBase);
            event_add(listener_event, NULL);

            // Create timer to cleanup stale channels
            event* cleanupEmptyChannels_event = evtimer_new(eventBase, &CleanupEmptyChannelsCallback, NULL);
            evtimer_add(cleanupEmptyChannels_event, &cleanupTimeout);
        }

        #ifdef USE_IPV6
        {
            // Create new listen socket, make it non-blocking and reusable
            listenSocketV6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

            evutil_make_socket_nonblocking(listenSocketV6);
            evutil_make_listen_socket_reuseable(listenSocketV6);


            #ifdef IPV6_V6ONLY
            {
                unsigned int nOne = 1;
                if (setsockopt(listenSocketV6, IPPROTO_IPV6, IPV6_V6ONLY, (void*)&nOne, sizeof(nOne)) != 0)
                {
                    ostringstream errorMessage;
                    errorMessage << errno << " - " << "Error setting IPV6_V6ONLY.";
                    FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", errorMessage);
                    return;

                }
            }
            #endif

            if (listenSocketV6 < 0)
            {
                FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", "Error opening IPv6 listen socket.");
                return;
            }

            sockaddr_in6 serverAddress6;
            serverAddress6.sin6_family = AF_INET6;
            serverAddress6.sin6_addr = in6addr_any;
            serverAddress6.sin6_port = htons(port);

            // Bind to socket
            if (bind(listenSocketV6, (struct sockaddr *) &serverAddress6, sizeof(serverAddress6)) < 0)
            {
                ostringstream errorMessage;
                errorMessage << errno << " - " << "Error binding to the IPv6 listening socket.";
                FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", errorMessage);
                return;
            }

            // Listen on socket
            if (listen(listenSocketV6, 5) < 0)
            {
                ostringstream errorMessage;
                errorMessage << errno << " - " << "Listening to the IPv6 listening socket.";
                FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "NotificationServer", errorMessage);
                return;
            }

            // Create new event to accept clients, add it
            event* listener_v6_event = event_new(eventBase, listenSocketV6, EV_READ|EV_PERSIST, &NotificationClientHandler::AcceptClient, (void*) eventBase);
            event_add(listener_v6_event, NULL);

        }
        #endif // USE_IPV6

        // Create timer to cleanup stale channels
        event* cleanupEmptyChannels_event = evtimer_new(eventBase, &CleanupEmptyChannelsCallback, NULL);
        evtimer_add(cleanupEmptyChannels_event, &cleanupTimeout);

        // Loop through all events (more will be added
        event_base_loop(eventBase, 0);
    }

    void ShutdownServer()
    {
        // Break the loop if event container is set
        if (eventBase)
            event_base_loopbreak(eventBase);

        // Cleanup clients and channels
        NotificationClientHandler::CleanupClients();
        NotificationChannel::CleanupChannels();

        // Cleanup socket if set
        if (listenSocket != -1)
        {
            close(listenSocket);
            listenSocket = -1;
        }

        #ifdef USE_IPV6
            if (listenSocketV6 != -1)
            {
                close(listenSocketV6);
                listenSocketV6 = -1;
            }
        #endif

        // Cleanup event container if set
        if (eventBase)
        {
            event_base_free(eventBase);
            eventBase = NULL;
        }

    }

    void CleanupEmptyChannelsCallback(evutil_socket_t nullSocket, short flags, void* socketParam)
    {
        NotificationChannel::CleanupEmptyChannels();
    }

}
