#include "config.h"

#include "NotificationClientHandler.h"

#include "NotificationChannel.h"


#include <sstream>
#include <time.h>

#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif

#ifdef HAVE_EVENT2_EVENT_H
    #include <event2/event.h>
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

/// Static constant member initializers
const int NotificationClientHandler::COMMAND_MAX_LENGTH = 0x10000;

/// Static member initializers
list<NotificationClientHandler*> NotificationClientHandler::clientList;
NotificationClientCommands NotificationClientHandler::commands;
uint64_t NotificationClientHandler::nextClientId = 0;

/// Private struct declarations
struct SendState
{
    event_base* eventBase;
    const void* data;
    int length;
    int position;
};


/// Public methods

NotificationClientHandler::~NotificationClientHandler()
{
    // Remove self from client list, unsubscribe from all channels
    clientList.remove(this);
    NotificationChannel::UnsubscribeFromAll(this);

    // Free process event and close socket
    event_free(processEvent);
    close(clientSocket);
}

void NotificationClientHandler::SendData(const void* data, int length)
{
    // Create new local state for each send event
    SendState* state = new SendState;
    state->eventBase = eventBase;
    state->data = data;
    state->length = length;
    state->position = 0;

    // Create send event and add to loop
    event* sendEvent = event_new(eventBase, clientSocket, EV_WRITE, &SendData, state);
    event_add(sendEvent, NULL);
}

void NotificationClientHandler::LogMessage(string message, FanoutLogger::MessageSeverity severity)
{
    // Log message (internally used for this client)
    ostringstream clientSource;
    clientSource << "NotificationClientHandler-" << clientId;
    FanoutLogger::LogMessage(severity, clientSource.str().c_str(), message);
}

void NotificationClientHandler::AcceptClient(evutil_socket_t listeningSocket, short flags, void* socketParam)
{
    event_base* eventBase =  (event_base*) socketParam;

    // Setup structs to accept with
    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    socklen_t clientAddressSize = sizeof(clientAddress);

    // Accept client
    int clientSocket = accept(listeningSocket, (struct sockaddr *) &clientAddress, &clientAddressSize);
    if (clientSocket < 0)
    {
        FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "NotificationClientHandler", "Error while accepting client socket.");
        return;
    }

    // Create new NotificationClientHandler and add it to the list
    NotificationClientHandler* client = new NotificationClientHandler(clientSocket, eventBase);
    clientList.push_back(client);
}

void NotificationClientHandler::CleanupClients()
{
    // Loop through client list and delete them
    for (list<NotificationClientHandler*>::iterator it = clientList.begin(); it != clientList.end(); ++it)
    {
        if (*it)
            delete (*it);
    }

    // Clear the list
    clientList.clear();
}


/// Private methods

NotificationClientHandler::NotificationClientHandler(int cSocket, event_base* cEventBase) : clientSocket(cSocket), eventBase(cEventBase)
{
    // Set clientId
    clientId = nextClientId;
    ++nextClientId;

    // Set socket non-blocking
    evutil_make_socket_nonblocking(clientSocket);

    // Add to all channel
    NotificationChannel::SubscribeToChannel(this, "all");

    // Setup process event
    processEvent = event_new(eventBase, clientSocket, EV_READ|EV_PERSIST, &NotificationClientHandler::ProcessData, (void*) this);
    event_add(processEvent, NULL);
}

// Static, used as callback function, automatically calls ProcessData funciton on as client handler
void NotificationClientHandler::ProcessData(evutil_socket_t clientSocket, short flags, void* processParam)
{
    // Call the ProcessData function on the client handler
    ((NotificationClientHandler*) processParam)->ProcessData();
}

void NotificationClientHandler::ProcessData()
{
    try
    {
        // Receive up to 256 bytes of data
        char recvBuffer[256];
        int recvLength = recv(clientSocket, recvBuffer, 256, 0);

        // Client properly disconnected
        if (recvLength == 0)
        {
            LogMessage("Client closed gracefully");
            delete this;
            return;
        }

        // Some error occurred
        if (recvLength < 0)
        {
            // Just return and try agian if it would block or asks to read again
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;

            // Otherwise we got a legitamate error
            throw "Error on recv from socket";
        }

        // Process received data
        bool newlineFound = false;
        int curX = 0;
        int lastX = 0;

        do
        {
            newlineFound = false;

            // Try to find the next newline
            for (int x = curX; x < recvLength; ++x)
            {
                if (recvBuffer[x] == '\n')
                {
                    newlineFound = true;
                    curX = x;
                    break;
                }
            }

            // Build the command if newline found
            if (newlineFound)
            {
                istringstream commandData(lastData.str() + string(recvBuffer + lastX, curX - lastX));
                string commandString;

                // Read the first word of the command out and handle it
                commandData >> commandString;
                if (!commands.HandleCommand(*this, commandString, commandData)) {
                    throw "Unknown command received";
                }

                // Set lastX to curX, reset lastData to a null string
                lastX = curX;
                lastData.str("");
            }

        } while (newlineFound);

        // Write out last data to a buffer
        if (lastX != recvLength)
            lastData.write(recvBuffer + lastX, recvLength - lastX);

        // Do not allow too much data to buffer up
        if (lastData.tellp() >= COMMAND_MAX_LENGTH)
            throw "Too much invalid data received";

    } catch (const char* message) {
        // Error ocurred, log message and delete self
        LogMessage(message, FanoutLogger::LOG_ERROR);
        delete this;
        return;
    }
}

void NotificationClientHandler::SendData(evutil_socket_t clientSocket, short flags, void* sendParam)
{
    SendState* state = ((SendState*) sendParam);

    // Try to send all the data
    int dataSent = send(clientSocket, ((const char*) state->data) + state->position, (state->length - state->position), 0);

    // Error occurred, close socket on next recv
    if (dataSent < 0)
    {
        // Add write event again if EAGAIN or EWOULDBLOCK
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            event* sendEvent = event_new(state->eventBase, clientSocket, EV_WRITE, &SendData, state);
            event_add(sendEvent, NULL);
            return;
        }

        // Delete state and return otherwise
        delete state;
        return;
    }

    // Add length if sent successfully
    state->position += dataSent;

    // Add write event again if there is still mroe data left
    if (state->position < state->length && state->position >= 0)
    {
        event* sendEvent = event_new(state->eventBase, clientSocket, EV_WRITE, &SendData, state);
        event_add(sendEvent, NULL);
        return;
    }

    // Otherwise just cleanup the state
    delete state;
    return;
}

