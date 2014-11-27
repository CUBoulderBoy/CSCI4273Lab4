#include "ppm.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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
        printf("recved %d chars\n", n);
        printf("%s\n", mesg_buf);
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
    char msg_buf[1024];
    struct sockaddr_in servaddr;
    socklen_t len;
    struct hostent *phe;    // pointer to host information entry

    msg->msgFlat(msg_buf);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    char* port = "32000";
    char* host = "localhost";

    // Map port number (char string) to port number (int)
    if ((servaddr.sin_port = htons((unsigned short)atoi(port))) == 0)
        errexit("can't get \"%s\" port number\n", port);

    // Map host name to IP address, allowing for dotted decimal
    if ( (phe = gethostbyname(host)) )
    {
        memcpy(&servaddr.sin_addr, phe->h_addr, phe->h_length);
    }
    else if ( (servaddr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
        errexit("can't get \"%s\" host entry\n", host);

    // Allocate a socket
    int upd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (upd_sock < 0)
        errexit("can't create socket: %s\n", strerror(errno));

    if (sendto(upd_sock, msg_buf, strlen(msg_buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        printf("Error with sendto %s\n", strerror(errno));
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
