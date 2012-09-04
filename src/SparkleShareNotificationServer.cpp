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
        while(1)
        {
            sleep(1);
        }
    } catch (const char* ex) {
        cout << ex << endl;
    }

    return 0;
}
