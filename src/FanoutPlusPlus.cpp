#include "config.h"

#include "FanoutLogger.h"
#include "NotificationServer.h"

#include <iostream>
#include <stdio.h>
#include <string>

#ifdef HAVE_GETOPT_H
    #include <getopt.h>
#endif

#ifdef HAVE_STDLIB_H
    #include <stdlib.h>
#endif

#ifdef HAVE_SYS_STAT_H
    #include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
    #include <unistd.h>
#endif

using namespace std;


void daemonize();
void displayUsage();

int main(int argc, char** argv)
{
    // Extern getopt stuff
    extern char* optarg;


    // Parse options
    static const char* optString = "bl:h?";
    static option optArray[] = {
        {"background", 0, NULL, 'b'},
        {"log-level", 1, NULL, 'l'},
        {"help", 0, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int optSupplied;
    int optIndex = 0;
    while ( (optSupplied = getopt_long(argc, argv, optString, optArray, &optIndex)) != -1)
    {
        // Convert to short option
        if (optSupplied == 0)
            optSupplied = optArray[optIndex].val;

        // Hanle short option
        switch (optSupplied)
        {
            // Match long argument
            case 'b':
            {
                daemonize();
                break;
            }

            case 'l':
            {
                int logLevel;
                istringstream loggingLevelParser(optarg);
                loggingLevelParser >> logLevel;

                if (loggingLevelParser.fail() || logLevel < 0 || logLevel > 4)
                {
                    cout << "Invalid value " << optarg << " specified for log-level." << endl;
                    displayUsage();
                    exit(1);
                }

                FanoutLogger::SetLoggingLevel(logLevel);
                break;
            }

            default:
            case 'h':
            case '?':
            {
                displayUsage();
                exit(0);
                break;
            }
        }
    }


    // Main loop
    try
    {
        // Start server on port 1986, loops indefinitely
        FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_INFO, "Main", "Server started.");
        NotificationServer::StartServer(1986);

        // Shutdown the server, never will actually get here
        NotificationServer::ShutdownServer();
        FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_INFO, "Main", "Server shutdown.");

    } catch (const char* ex) {
        FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "Main", ex);
        NotificationServer::ShutdownServer();
        return 1;

    } catch (string ex) {
        FanoutLogger::LogMessage(FanoutLogger::FANOUT_LOG_ERROR, "Main", ex);
        NotificationServer::ShutdownServer();
        return 1;
    }

    return 0;
}


void daemonize()
{
    // No need to fork if no parent process
    pid_t pid, sid;
    if ( getppid() == 1 ) return;

    // Fork and exit success if parent or failure if no fork
    pid = fork();
    if (pid < 0)
        exit(1);

    if (pid > 0)
        exit(0);

    // Set umask to 0, not sure why this is required, leaving out for now
    // umask(0);

    // Create new session so child has no controlling terminal
    sid = setsid();
    if (sid < 0)
        exit(1);

    // Change to root to prevent from holding a filesystem lock
    if (chdir("/") < 0)
        exit(1);

    // Reopen stdin, stdout, and stderr to /dev/null
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}

void displayUsage()
{
    cout << PACKAGE_NAME << " v" << PACKAGE_VERSION << endl;
    cout << endl;
    cout << "Usage: " << PACKAGE_NAME << " [OPTION]" << endl;
    cout << endl;
    cout << "  " << "-b, --background\t\tBackground process, fully daemonize process." << endl;
    cout << "  " << "-l [#], --log-level [#]\tSet logging level, valid values 0 (Silent) to 4 (FANOUT_LOG_DEBUG)" << endl;
    cout << "  " << "-h, --help\t\t\tShow this usage message." << endl;
    cout << endl;
    cout << "Current source can be found at <" << PACKAGE_SOURCE_URL << ">." << endl;

}

