#include "ppp.h"
#include "message.h"
#include <unistd.h>

#define SLEEP_USEC 50

char* msg_text = "The goal of this programming assignment is to evaluate two network implementation models......\n";

void* ftp_app(void* arg);
void* telnet_app(void* arg);
void* rdp_app(void* arg);
void* dns_app(void* arg);

// Struct for sending to pipes
struct pipe_unit {
    int protocol_id;
    Message *msg;
};

void* ftp_app(void* arg)
{
    PPP* ppp = (PPP*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg_text, 100);
        pipe_unit ftp;
        ftp.protocol_id = 5;
        ftp.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->ftp_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(ppp->ftp_send_pipe.pipe_d[1], (char*) &ftp, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->ftp_send_pipe.pipe_mutex);
        usleep(SLEEP_USEC);
    }
}

void* telnet_app(void* arg)
{
    PPP* ppp = (PPP*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg_text, 100);
        pipe_unit tel;
        tel.protocol_id = 6;
        tel.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->tel_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(ppp->tel_send_pipe.pipe_d[1], (char*) &tel, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->tel_send_pipe.pipe_mutex);
        usleep(SLEEP_USEC);
    }
}

void* rdp_app(void* arg)
{
    PPP* ppp = (PPP*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg_text, 100);
        pipe_unit rdp;
        rdp.protocol_id = 5;
        rdp.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->rdp_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(ppp->rdp_send_pipe.pipe_d[1], (char*) &rdp, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->rdp_send_pipe.pipe_mutex);
        usleep(SLEEP_USEC);
    }
}

void* dns_app(void* arg)
{
    PPP* ppp = (PPP*) arg;
    for (int i = 0; i < 100; i++)
    {
        Message* msg = new Message(msg_text, 100);
        pipe_unit dns;
        dns.protocol_id = 5;
        dns.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->dns_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(ppp->dns_send_pipe.pipe_d[1], (char*) &dns, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->dns_send_pipe.pipe_mutex);
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
        send_port = "32001";
        recv_port = "32000";        
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
    pthread_t thread[4];
    PPP* ppp = new PPP(send_port, recv_port);
    sleep(5);

    err = pthread_create(&thread[0], NULL, ftp_app, (void*) ppp);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&thread[1], NULL, telnet_app, (void*) ppp);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&thread[2], NULL, rdp_app, (void*) ppp);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    err = pthread_create(&thread[3], NULL, dns_app, (void*) ppp);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pthread_join(thread[3], NULL);

    while(1){
        sleep(1);
    }

    return 0;
}