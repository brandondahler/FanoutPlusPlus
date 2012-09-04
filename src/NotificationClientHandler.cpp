#include "NotificationClientHandler.h"

#include "NotificationServer.h"

#include <iostream>
#include <sstream>
#include <time.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>

#ifdef WIN32
   #include <winsock2.h>
#else
   #include <sys/socket.h>
   #include <sys/un.h>
#endif


using namespace std;

NotificationClientHandler::NotificationClientHandler(socket_t cSocket)
{
    pthread_mutex_init(&socketMutex, 0);
    clientSocket = cSocket;
    pthread_create(&clientThread, NULL, &ProcessDataThread, (void*) this);
}

NotificationClientHandler::~NotificationClientHandler()
{
    close(clientSocket);
    pthread_cancel(clientThread);
    pthread_mutex_destroy(&socketMutex);
}

void NotificationClientHandler::SendData(const void* data, unsigned int length)
{
    pthread_mutex_lock(&socketMutex);

    send(clientSocket, (const char*) data, length, 0);

    pthread_mutex_unlock(&socketMutex);
}

void NotificationClientHandler::ProcessData()
{
    cout << "Processing data " << endl;
    NotificationServer::SubscribeToChannel(this, "all");

    try
    {


        while (1)
        {
            char commandBuffer[256];
            int commandIndex = 0;

            // Wait until we have some command in our buffer
            do
            {
                char peekBuffer[256];

                // Make sure there's atleast something
                recv(clientSocket, peekBuffer, 1, MSG_PEEK);

                int peekLength = recv(clientSocket, peekBuffer, 256, MSG_PEEK);

                bool commandReceived = false;
                for (int x = 0; x < peekLength; ++x)
                {
                    if (peekBuffer[x] == '\n')
                    {
                        // Too long to receive
                        if (256 - (commandIndex + x + 1) < 0) throw "Invalid data received.";

                        recv(clientSocket, &commandBuffer[commandIndex], x + 1, 0);
                        commandIndex += x + 1;
                        commandReceived = true;
                        break;
                    }
                }

                if (commandReceived)
                    break;

                // Too long to receive
                if (256 - (commandIndex + peekLength) < 0) throw "Invalid data received.";

                int recvLength = recv(clientSocket, &commandBuffer[commandIndex], peekLength, 0);
                if (recvLength != peekLength)
                    throw "Error reading command data.";

                commandIndex += recvLength;
            } while (commandIndex > 0 && commandBuffer[commandIndex] != '\n' && commandIndex < 256);

            if (commandIndex <= 0) throw "Error reading command.";
            if (commandIndex > 256) throw "Buffer overflow while reading command.";

            istringstream oss(string(commandBuffer, commandIndex));
            string commandString;

            pthread_mutex_lock(&socketMutex);

            oss >> commandString;
            if (commandString == "ping") {
                cout << "pong" << endl;
                stringstream timeStringStream;
                timeStringStream << (int) time(NULL) << "000";

                string timeString;
                timeStringStream >> timeString;
                timeString += '\n';

                send(clientSocket, timeString.c_str(), timeString.length(), 0);

            } else if (commandString == "subscribe") {
                cout << "subscribing" << endl;
                string channel;
                oss >> channel;

                NotificationServer::SubscribeToChannel(this, channel);

            } else if (commandString == "unsubscribe") {
                cout << "unsubscribing" << endl;

                string channel;
                oss >> channel;

                NotificationServer::UnsubscribeFromChannel(this, channel);

            } else if (commandString == "announce") {
                cout << "announcing" << endl;

                string channel;
                string announceHash;

                oss >> channel >> announceHash;

                NotificationServer::AnnounceToChannel(this, channel, announceHash);

            } else {
                pthread_mutex_unlock(&socketMutex);
                throw "Unknown command received.";
            }

            pthread_mutex_unlock(&socketMutex);
        }
    } catch (const char* message) {

    }
}

void* NotificationClientHandler::ProcessDataThread(void* param)
{
    NotificationClientHandler* client = (NotificationClientHandler*) param;
    client->ProcessData();

    NotificationServer::ClientClose(client);

    pthread_exit(0);
    return 0;
}
