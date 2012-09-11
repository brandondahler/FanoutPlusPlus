#include "config.h"

#include "FanoutLogger.h"
#include "NotificationServer.h"

#include <string>

#ifdef WIN32
    #include <windows.h>
    #define sleep(n) Sleep(1000 * n)
#endif

using namespace std;

int main()
{
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
