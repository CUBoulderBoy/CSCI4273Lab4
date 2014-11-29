#include "ppm.h"
#include <unistd.h>

int main(int argc, char**argv)
{
    char* send_port;
    char* recv_port;

    switch (argc)
    {
    case 1:
        send_port = "32001";
        recv_port = "32000";        
        break;

    case 3:
        send_port = argv[1];
        recv_port = argv[2];
        break;

    default:
        printf("Error. Must use no args and the default in and out port of 32001 and 32000, or provide both in and out ports\n");
        exit(0);
    }

    PPM* ppm = new PPM(send_port, recv_port);

    while(1){
        sleep(1);
    }
}