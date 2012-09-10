#include "config.h"

#include "NotificationClientHandler.h"

#include "NotificationServer.h"
#include "StringHelper.h"

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

/*
 * Static constant member initializers
 */
const int NotificationClientHandler::COMMAND_MAX_LENGTH = 0x10000;

/*
 * Static member initializers
 */
NotificationClientCommands NotificationClientHandler::commands;
pthread_mutex_t NotificationClientHandler::clientIdMutex = PTHREAD_MUTEX_INITIALIZER;
uint64_t NotificationClientHandler::nextClientId = 0;


/*
 * Public methods
 */
NotificationClientHandler::NotificationClientHandler(socket_t cSocket)
{
    pthread_mutex_lock(&clientIdMutex);
    clientId = nextClientId;
    ++nextClientId;
    pthread_mutex_unlock(&clientIdMutex);

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

void NotificationClientHandler::LogMessage(string message)
{
    cout << clientId << " :: " << message << endl;
}

/*
 * Private methods
 */

void NotificationClientHandler::ProcessData()
{
    cout << "Processing data for client " << clientId << "." << endl;
    NotificationServer::SubscribeToChannel(this, "all");

    try
    {
        ostringstream nextBuffer;

        while (1)
        {
            int commandBufferLength = 0;
            ostringstream commandBuffer;

            // Copy the nextBuffer into commandBuffer
            string nextBufferString = nextBuffer.str();

            commandBuffer << nextBufferString;
            commandBufferLength += nextBufferString.length();

            // Clear nextBuffer out
            nextBuffer.str("");
            nextBuffer.clear();

            while (1)
            {
                char recvBuffer[256];
                int recvLength = recv(clientSocket, recvBuffer, 256,0);

                if (recvLength <= 0)
                {
                    return;
                }

                // Find new line, break out of while loop when found
                string recvBufferString(recvBuffer, recvLength);
                size_t newLinePos = recvBufferString.find('\n');
                if (newLinePos != string::npos)
                {
                    commandBuffer << recvBufferString.substr(0, newLinePos);
                    nextBuffer << recvBufferString.substr(newLinePos);
                    break;
                }

                // Do not allow too much data to buffer up
                if (commandBufferLength >= COMMAND_MAX_LENGTH)
                    throw string("Too much invalid data received from client ") + StringHelper::ToString(clientId) + ".";
            }

            istringstream commandData(commandBuffer.str());
            string commandString;

            commandData >> commandString;
            if (!commands.HandleCommand(*this, commandString, commandData)) {
                pthread_mutex_unlock(&socketMutex);
                throw string("Unknown command received from client ") + StringHelper::ToString(clientId) + ".";
            }
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
