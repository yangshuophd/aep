#include "connect_test_server.h"

int main(int argc,char* argv[])
{
    int port = atoi(argv[1]);
    ConnectTestServer server;
    server.Init();
    server.Listen(port);
    return 0;
}
