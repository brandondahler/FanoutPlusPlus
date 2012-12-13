#include "config.h"

#include "FanoutLogger.h"
#include "NotificationServer.h"

#include <string>

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

int main(int argc, const char* argv[])
{
    if (argc > 1)
    {
        for (int x = 1; x < argc; ++x)
        {
            if (string(argv[x]) ==  "-b")
                daemonize();
        }
    }

    try
    {
        NotificationServer::StartServer(1986);

        /// TODO: Anything that is non-essential can be done in this thread.
        ///   When done with whatever, we can just wait for a natural shutdown.
        FanoutLogger::LogMessage(FanoutLogger::LOG_INFO, "Main", "Waiting for shutdown...");
        NotificationServer::WaitForServerShutdown();
        FanoutLogger::LogMessage(FanoutLogger::LOG_INFO, "Main", "Shutdown completed");

    } catch (const char* ex) {
        FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "Main", ex);
        return 1;
    } catch (string ex) {
        FanoutLogger::LogMessage(FanoutLogger::LOG_ERROR, "Main", ex);
        return 1;
    }

    return 0;
}


void daemonize()
{
    pid_t pid, sid;
    if ( getppid() == 1 ) return;

    pid = fork();
    if (pid < 0)
        exit(1);

    if (pid > 0)
        exit(0);

    umask(0);

    sid = setsid();
    if (sid < 0)
        exit(1);

    if ((chdir("/")) < 0)
        exit(1);

    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}
