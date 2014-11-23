#include "ppm.h"
#include <pthread.h>
#include <sys/socket.h>
#include "updsocket.cpp"

void ethernet_recv(void* msg);

void ethernet_recv(void* msg)
{

}


void* PPM::read_upd(void* arg)
{
    PPM* ppm = (PPM*) arg;
    char udp_portnum[6] = "32000";
    int upd_sock = updSocket(udp_portnum);
    char mesg_buf[1024];
    struct sockaddr_in cliaddr;
    socklen_t len;
    int n;

    while (1) {
        memset(&mesg_buf, 0, sizeof(mesg_buf));
        len = sizeof(cliaddr);
        n = recvfrom(upd_sock, mesg_buf, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        Message* msg = new Message(mesg_buf, n);
        ppm->m_thread_pool->dispatch_thread(ethernet_recv, (void*) msg);
    }
}

PPM::PPM()
{
    m_thread_pool = new ThreadPool(25);

    // start a thread to listen to UPD (ethernet) messages
    pthread_t tid;
    int err = pthread_create(&tid, NULL, PPM::read_upd, (void*) this);
    if (err != 0) {
        cout << "pthread_create() failed: " << err << endl;
        exit(1);
    }
}

PPM::~PPM()
{
    delete m_thread_pool;
}

void PPM::ethernet_send(int protocol_id, Message* msg)
{

}

// void PPM::ethernet_recv(Message* msg)
// void PPM::ethernet_recv((void*) msg)
// {

// }

void PPM::IP_send(int protocol_id, Message* msg)
{

}

void PPM::IP_recv(Message* msg)
{

}

void PPM::TCP_send(int protocol_id, Message* msg)
{

}

void PPM::TCP_recv(Message* msg)
{

}

void PPM::UDP_send(int protocol_id, Message* msg)
{

}

void PPM::UDP_recv(Message* msg)
{

}

void PPM::FTP_send(int protocol_id, Message* msg)
{

}

void PPM::FTP_recv(Message* msg)
{

}

void PPM::telnet_send(int protocol_id, Message* msg)
{

}

void PPM::telnet_recv(Message* msg)
{

}

void PPM::RDP_send(int protocol_id, Message* msg)
{

}

void PPM::RDP_recv(Message* msg)
{

}

void PPM::DNS_send(int protocol_id, Message* msg)
{

}

void PPM::DNS_recv(Message* msg)
{

}
