#include "ppm.h"
#include "protocolstructs.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "updsocket.cpp"

void ethernet_recv(void* msg);

void ethernet_recv(void* msg)
{
    eth_header* stripped = (eth_header*)msg->msgStripHdr(sizeof(eth_header));
    int protocol_id = stripped->hlp;
    if (protocol_id == IP_ID)
        IP_recv(msg);
    else
        printf("Invalid protocol id %d in ethernet_recv\n", protocol_id);
}


void* PPM::read_upd(void* arg)
{
    PPM* ppm = (PPM*) arg;
    char* udp_portnum = ppm->m_recv_port;
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

PPM::PPM(char* send_port, char* recv_port)
{
    m_thread_pool = new ThreadPool(25);
    m_send_port = send_port;
    m_recv_port = recv_port;

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

    eth_header *header = (eth_header *) malloc(sizeof(eth_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(eth_header));
    msg->msgFlat(msg_buf);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    char* host = "localhost";

    // Map port number (char string) to port number (int)
    if ((servaddr.sin_port = htons((unsigned short)atoi(m_send_port))) == 0)
        errexit("can't get \"%s\" port number\n", m_send_port);

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
    IP_header *header = (IP_header *) malloc(sizeof(IP_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(IP_header));
    ethernet_send(IP_ID, msg);
}

void PPM::IP_recv(Message* msg)
{
    IP_header* stripped = (IP_header*)msg->msgStripHdr(sizeof(IP_header));
    int protocol_id = stripped->hlp;
    if (protocol_id == TCP_ID)
        TCP_recv(msg);
    else if (protocol_id == UDP_ID)
        UDP_recv(msg);
    else
        printf("Invalid protocol id %d in IP_recv\n", protocol_id);
}

void PPM::TCP_send(int protocol_id, Message* msg)
{
    TCP_header *header = (TCP_header *) malloc(sizeof(TCP_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(TCP_header));
    IP_send(TCP_ID, msg);
}

void PPM::TCP_recv(Message* msg)
{
    TCP_header* stripped = (TCP_header*)msg->msgStripHdr(sizeof(TCP_header));
    int protocol_id = stripped->hlp;
    if (protocol_id == FTP_ID)
        FTP_recv(msg);
    else if (protocol_id == TELNET_ID)
        telnet_recv(msg);
    else
        printf("Invalid protocol id %d in TCP_recv\n", protocol_id);
}

void PPM::UDP_send(int protocol_id, Message* msg)
{
    UDP_header *header = (UDP_header *) malloc(sizeof(UDP_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(UDP_header));
    IP_send(UDP_ID, msg);
}

void PPM::UDP_recv(Message* msg)
{
    UDP_header* stripped = (UDP_header*)msg->msgStripHdr(sizeof(UDP_header));
    int protocol_id = stripped->hlp;
    if (protocol_id == RDP_ID)
        RDP_recv(msg);
    else if (protocol_id == DNS_ID)
        DNS_recv(msg);
    else
        printf("Invalid protocol id %d in UDP_recv\n", protocol_id);
}

void PPM::FTP_send(int protocol_id, Message* msg)
{
    ftp_header *header = (ftp_header *) malloc(sizeof(ftp_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(ftp_header));
    TCP_send(FTP_ID, msg);
}

void PPM::FTP_recv(Message* msg)
{

}

void PPM::telnet_send(int protocol_id, Message* msg)
{
    tel_header *header = (tel_header *) malloc(sizeof(tel_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(tel_header));
    TCP_send(TELNET_ID, msg);
}

void PPM::telnet_recv(Message* msg)
{

}

void PPM::RDP_send(int protocol_id, Message* msg)
{
    RDP_header *header = (RDP_header *) malloc(sizeof(RDP_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(RDP_header));
    UDP_send(RDP_ID, msg);
}

void PPM::RDP_recv(Message* msg)
{

}

void PPM::DNS_send(int protocol_id, Message* msg)
{
    ftp_header *header = (ftp_header *) malloc(sizeof(ftp_header));
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(ftp_header));
    UDP_send(FTP_ID, msg);
}

void PPM::DNS_recv(Message* msg)
{

}
