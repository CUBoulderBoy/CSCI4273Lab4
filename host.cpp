#include "ppp.h"
#include "ppm.h"
#include "message.h"
#include "protocolstructs.h"
#include <unistd.h>

#define SLEEP_USEC 50

char* msg = "The goal of this programming assignment is to evaluate two network implementation models.......";

void ftp_app(void* arg)
{
    PPM ppm = (PPM*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg, 100);
        ppm.ftp_send(FTP_ID, msg);
        usleep(SLEEP_USEC);
    }
}

void telnet_app(void* arg)
{
    PPM ppm = (PPM*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg, 100);
        ppm.telnet_send(TELNET_ID, msg);
        usleep(SLEEP_USEC);
    }
}

void rdp_app(void* arg)
{
    PPM ppm = (PPM*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg, 100);
        ppm.RDP_send(RDP_ID, msg);
        usleep(SLEEP_USEC);
    }
}

void dns_app(void* arg)
{
    PPM ppm = (PPM*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg, 100);
        ppm.DNS_send(DNS_ID, msg);
        usleep(SLEEP_USEC);
    }
}

int main(int argc, char const *argv[])
{
    int err;
    pthread_t tids[4];
    PPM ppm = new PPM("32000", "32001");

    err = pthread_create(&tid, NULL, ftp_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&tid, NULL, telnet_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&tid, NULL, rdp_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&tid, NULL, dns_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    return 0;
}