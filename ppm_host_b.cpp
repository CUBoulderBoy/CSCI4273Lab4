#include "ppm.h"
#include "message.h"
#include "protocolstructs.h"
#include <unistd.h>
#include <sys/time.h> 

#define SLEEP_USEC  2
#define MSG_LEN    100
#define NUM_MSG    100

int m_num_recv;

char* msg_text = "The goal of this programming assignment is to evaluate two network implementation models.....\n";

void* ftp_app(void* arg);
void* telnet_app(void* arg);
void* rdp_app(void* arg);
void* dns_app(void* arg);

void* ftp_app(void* arg)
{
    PPM* ppm = (PPM*) arg;
    for (int i = 0; i < NUM_MSG; i++)
    {
        Message* msg = new Message(msg_text, MSG_LEN);
        ppm->FTP_send(FTP_ID, msg);
        usleep(SLEEP_USEC);
    }
}

void* telnet_app(void* arg)
{
    PPM* ppm = (PPM*) arg;
    for (int i = 0; i < NUM_MSG; i++)
    {
        Message* msg = new Message(msg_text, MSG_LEN);
        ppm->telnet_send(TELNET_ID, msg);
        usleep(SLEEP_USEC);
    }
}

void* rdp_app(void* arg)
{
    PPM* ppm = (PPM*) arg;
    for (int i = 0; i < NUM_MSG; i++)
    {
        Message* msg = new Message(msg_text, MSG_LEN);
        ppm->RDP_send(RDP_ID, msg);
        usleep(SLEEP_USEC);
    }
}

void* dns_app(void* arg)
{
    PPM* ppm = (PPM*) arg;
    for (int i = 0; i < NUM_MSG; i++)
    {
        Message* msg = new Message(msg_text, MSG_LEN);
        ppm->DNS_send(DNS_ID, msg);
        usleep(SLEEP_USEC);
    }
}

int main(int argc, char**argv)
{
    char* send_port;
    char* recv_port;

    switch (argc)
    {
    case 1:
        send_port = "32000";
        recv_port = "32001";        
        break;

    case 3:
        send_port = argv[1];
        recv_port = argv[2];
        break;

    default:
        printf("Error. Must use no args and the default in and out port of 32000 and 32001, or provide both in and out ports\n");
        exit(0);
    }

    int err;
    m_num_recv = 0;
    pthread_t thread[4];
    PPM* ppm = new PPM(send_port, recv_port);
    sleep(1);

    struct timeval tim;
    gettimeofday(&tim, NULL);
    double t1 = tim.tv_sec + (tim.tv_usec/1000000.0);

    err = pthread_create(&thread[0], NULL, ftp_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&thread[1], NULL, telnet_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&thread[2], NULL, rdp_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&thread[3], NULL, dns_app, (void*) ppm);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pthread_join(thread[3], NULL);

    gettimeofday(&tim, NULL);
    double t2 = tim.tv_sec + (tim.tv_usec/1000000.0);

    while (1) {
        if (m_num_recv >= 400)
            break;
    }

    gettimeofday(&tim, NULL);
    double t3 = tim.tv_sec + (tim.tv_usec/1000000.0);

    printf("%.6lf seconds elapsed for sending the messages\n", t2 - t1);
    printf("%.6lf seconds elapsed for receiving the messages\n", t3 - t1);

    return 0;
}