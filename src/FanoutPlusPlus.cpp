#include <iostream>

#include "NotificationServer.h"

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
        ///   When done with whatever, we can jus wait for a natural shutdown.
        cout << "Waiting for shutdown..." << endl;
        NotificationServer::WaitForServerShutdown();
        cout << "Shutdown completed" << endl;

    } catch (const char* ex) {
        cout << ex << endl;
    } catch (string ex) {
        cout << ex << endl;
    }

    return 0;
}
