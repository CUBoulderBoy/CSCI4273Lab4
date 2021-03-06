#include "ppm.h"
#include "protocolstructs.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "updsocket.cpp"

#define LOGGING 0
#define PRINT_MSG_RECV 1

extern int m_num_recv;

void PPM::ethernet_recv(void* arg)
{
    if (LOGGING) printf("eth recv called\n");

    Message* msg = (Message*) arg;

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

    struct sockaddr_in cliaddr;
    socklen_t len;
    int n;

    while (1) {
        char* msg_buf = new char[1024]; // TODO fix memory leak
        memset(msg_buf, 0, 1024);

        len = sizeof(cliaddr);
        n = recvfrom(upd_sock, msg_buf, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        if (LOGGING) printf("read_UDP received %d chars\n", n);
        Message* msg = new Message(msg_buf, n);
        while (ppm->m_thread_pool->dispatch_thread(PPM::ethernet_recv, (void*) msg) < 0) {}
        //ppm->m_num_recv++;
        // delete[] msg_buf;
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
    if (LOGGING) printf("eth send called\n");

    struct sockaddr_in servaddr;
    socklen_t len;
    struct hostent *phe;    // pointer to host information entry

    char* msg_buf = new char[1024];

    eth_header *h = new eth_header;
    h->hlp = protocol_id;
    h->m_size = msg->msgLen();

    msg->msgAddHdr((char*) h, sizeof(eth_header));
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

    m_mutex.lock();
    if (sendto(upd_sock, msg_buf, msg->msgLen(), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        printf("Error with sendto %s\n", strerror(errno));
    m_mutex.unlock();

    m_num_sent++;

    delete[] msg_buf;
    delete msg;
    delete h;
}

void PPM::IP_send(int protocol_id, Message* msg)
{
    if (LOGGING) printf("IP send called\n");

    IP_header *header = new IP_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(IP_header));
    ethernet_send(IP_ID, msg);

    delete header;
}

void PPM::IP_recv(Message* msg)
{
    if (LOGGING) printf("IP_recv called\n");

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
    if (LOGGING) printf("TCP send called\n");

    TCP_header *header = new TCP_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(TCP_header));
    IP_send(TCP_ID, msg);

    delete header;
}

void PPM::TCP_recv(Message* msg)
{
    if (LOGGING) printf("TCP_recv called\n");

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
    if (LOGGING) printf("UDP_send called\n");

    UDP_header *header = new UDP_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(UDP_header));
    IP_send(UDP_ID, msg);

    delete header;
}

void PPM::UDP_recv(Message* msg)
{
    if (LOGGING) printf("UDP_recv called\n");

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
    if (LOGGING) printf("FTP_send called\n");

    ftp_header *header = new ftp_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(ftp_header));
    TCP_send(FTP_ID, msg);

    delete header;
}

void PPM::FTP_recv(Message* msg)
{
    char buf[1024];
    ftp_header* stripped = (ftp_header*)msg->msgStripHdr(sizeof(ftp_header));
    msg->msgFlat(buf);
    if (PRINT_MSG_RECV) printf("ftp recieved message %s\n", buf);
    delete msg;
    m_num_recv++;
}

void PPM::telnet_send(int protocol_id, Message* msg)
{
    if (LOGGING) printf("telnet_send called\n");

    tel_header *header = new tel_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(tel_header));
    TCP_send(TELNET_ID, msg);

    delete header;
}

void PPM::telnet_recv(Message* msg)
{
    char buf[1024];
    tel_header* stripped = (tel_header*)msg->msgStripHdr(sizeof(tel_header));
    msg->msgFlat(buf);
    if (PRINT_MSG_RECV) printf("telnet recieved message %s\n", buf);
    delete msg;
    m_num_recv++;
}

void PPM::RDP_send(int protocol_id, Message* msg)
{
    if (LOGGING) printf("RDP_send called\n");

    RDP_header *header = new RDP_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(RDP_header));
    UDP_send(RDP_ID, msg);

    delete header;
}

void PPM::RDP_recv(Message* msg)
{
    char buf[1024];
    RDP_header* stripped = (RDP_header*)msg->msgStripHdr(sizeof(RDP_header));
    msg->msgFlat(buf);
    if (PRINT_MSG_RECV) printf("RDP recieved message %s\n", buf);
    delete msg;
    m_num_recv++;
}

void PPM::DNS_send(int protocol_id, Message* msg)
{
    if (LOGGING) printf("DNS_send called\n");

    DNS_header *header = new DNS_header;
    header->hlp = protocol_id;
    header->m_size = msg->msgLen();

    msg->msgAddHdr((char *)header, sizeof(DNS_header));
    UDP_send(DNS_ID, msg);

    delete header;
}

void PPM::DNS_recv(Message* msg)
{
    char buf[1024];
    DNS_header* stripped = (DNS_header*)msg->msgStripHdr(sizeof(DNS_header));
    msg->msgFlat(buf);
    if (PRINT_MSG_RECV) printf("DNS recieved message %s\n", buf);
    delete msg;
    m_num_recv++;
}
