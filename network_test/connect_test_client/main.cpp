#include "connect_test_client.h"

int main(int argc,char* argv[])
{
    const char *hostName = argv[1];
    int port = atoi(argv[2]);
    ConnectTestClient client(0);
    client.Init();
    client.Connect(hostName, port);
    client.Run();
    client.Shut();
    return 0;
}
