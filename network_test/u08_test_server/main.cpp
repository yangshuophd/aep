#include "u08_test_server.h"

int main(int argc,char* argv[])
{
    int port = atoi(argv[1]);
    U08TestServer server;
    server.Init();
    server.Listen(port);
    return 0;
}
