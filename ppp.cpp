#include "ppp.h"
#include "updsocket.cpp"

#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

struct pipe_unit {
    int protocol_id;
    Message *msg;
};

PPP::PPP(char in[], char out[]){
    // Start communications on sockets first
    start_com(in, out);

    // For testing
    cout << "Building pipes" << endl;

    // Create pipes for threads
    pipe(ftp_send_pipe.pipe_d);
    pipe(ftp_recv_pipe.pipe_d);

    pipe(tel_send_pipe.pipe_d);
    pipe(tel_recv_pipe.pipe_d);

    pipe(tcp_send_pipe.pipe_d);
    pipe(tcp_recv_pipe.pipe_d);

    pipe(rdp_send_pipe.pipe_d);
    pipe(rdp_recv_pipe.pipe_d);

    pipe(udp_send_pipe.pipe_d);
    pipe(udp_recv_pipe.pipe_d);

    pipe(dns_send_pipe.pipe_d);
    pipe(dns_recv_pipe.pipe_d);

    pipe(eth_send_pipe.pipe_d);
    pipe(eth_recv_pipe.pipe_d);

    pipe(ip_send_pipe.pipe_d);
    pipe(ip_recv_pipe.pipe_d);

    // For testing
    cout << "Initializing mutex locks" << endl;

    // Initialize mutex locks on pipes
    ftp_send_mut = PTHREAD_MUTEX_INITIALIZER;
    ftp_send_pipe.pipe_mutex = &ftp_send_mut;
    ftp_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    ftp_recv_pipe.pipe_mutex = &ftp_recv_mut;

    tel_send_mut = PTHREAD_MUTEX_INITIALIZER;
    tel_send_pipe.pipe_mutex = &tel_send_mut;
    tel_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    tel_recv_pipe.pipe_mutex = &tel_recv_mut;

    rdp_send_mut = PTHREAD_MUTEX_INITIALIZER;
    rdp_send_pipe.pipe_mutex = &rdp_send_mut;
    rdp_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    rdp_recv_pipe.pipe_mutex = &rdp_recv_mut;

    dns_send_mut = PTHREAD_MUTEX_INITIALIZER;
    dns_send_pipe.pipe_mutex = &dns_send_mut;
    dns_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    dns_recv_pipe.pipe_mutex = &dns_recv_mut;

    tcp_send_mut = PTHREAD_MUTEX_INITIALIZER;
    tcp_send_pipe.pipe_mutex = &tcp_send_mut;
    tcp_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    tcp_recv_pipe.pipe_mutex = &tcp_recv_mut;

    udp_send_mut = PTHREAD_MUTEX_INITIALIZER;
    udp_send_pipe.pipe_mutex = &udp_send_mut;
    udp_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    udp_recv_pipe.pipe_mutex = &udp_recv_mut;

    ip_send_mut = PTHREAD_MUTEX_INITIALIZER;
    ip_send_pipe.pipe_mutex = &ip_send_mut;
    ip_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    ip_recv_pipe.pipe_mutex = &ip_recv_mut;

    eth_send_mut = PTHREAD_MUTEX_INITIALIZER;
    eth_send_pipe.pipe_mutex = &eth_send_mut;
    eth_recv_mut = PTHREAD_MUTEX_INITIALIZER;
    eth_recv_pipe.pipe_mutex = &eth_recv_mut;

    // For testing
    cout << "Creating thread pool" << endl;

    // Create thread pool
    m_thread_pool = new ThreadPool(16);

    // Create thread ids
    pthread_t tid_1, tid_2, tid_3, tid_4, tid_5, tid_6, tid_7, tid_8, tid_9;
    pthread_t tid_10, tid_11, tid_12, tid_13, tid_14, tid_15, tid_16, tid_17;

    // Create vars for threads
    int err;

    // For testing
    cout << "Spooling up threads" << endl;

    // start a thread for receiving messages
    err = pthread_create(&tid_17, NULL, PPP::msg_recv, (void*) this);
    if (err != 0) {
        cout << "msg_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ethernet up
    err = pthread_create(&tid_1, NULL, PPP::eth_recv, (void*) this);
    if (err != 0) {
        cout << "eth_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ethernet down
    err = pthread_create(&tid_2, NULL, PPP::eth_send, (void*) this);
    if (err != 0) {
        cout << "eth_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ip up
    err = pthread_create(&tid_3, NULL, PPP::IP_recv, (void*) this);
    if (err != 0) {
        cout << "IP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ip down
    err = pthread_create(&tid_4, NULL, PPP::IP_send, (void*) this);
    if (err != 0) {
        cout << "IP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for udp up
    err = pthread_create(&tid_5, NULL, PPP::UDP_recv, (void*) this);
    if (err != 0) {
        cout << "UDP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for udp down
    err = pthread_create(&tid_6, NULL, PPP::UDP_send, (void*) this);
    if (err != 0) {
        cout << "UDP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for tcp up
    err = pthread_create(&tid_7, NULL, PPP::TCP_recv, (void*) this);
    if (err != 0) {
        cout << "TCP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for tcp down
    err = pthread_create(&tid_8, NULL, PPP::TCP_send, (void*) this);
    if (err != 0) {
        cout << "TCP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ftp up
    err = pthread_create(&tid_9, NULL, PPP::FTP_recv, (void*) this);
    if (err != 0) {
        cout << "FTP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ftp down
    err = pthread_create(&tid_10, NULL, PPP::FTP_send, (void*) this);
    if (err != 0) {
        cout << "FTP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for tel up
    err = pthread_create(&tid_11, NULL, PPP::tel_recv, (void*) this);
    if (err != 0) {
        cout << "tel_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for tel down
    err = pthread_create(&tid_12, NULL, PPP::tel_send, (void*) this);
    if (err != 0) {
        cout << "tel_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for rdp up
    err = pthread_create(&tid_13, NULL, PPP::RDP_recv, (void*) this);
    if (err != 0) {
        cout << "RDP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for rdp down
    err = pthread_create(&tid_14, NULL, PPP::RDP_send, (void*) this);
    if (err != 0) {
        cout << "RDP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for dns up
    err = pthread_create(&tid_15, NULL, PPP::DNS_recv, (void*) this);
    if (err != 0) {
        cout << "dns_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for dns down
    err = pthread_create(&tid_16, NULL, PPP::DNS_send, (void*) this);
    if (err != 0) {
        cout << "dns_send() failed: " << err << endl;
        exit(1);
    }

    // For testing
    cout << "Threads all spooled up" << endl;
}

PPP::~PPP()
{
    delete m_thread_pool;
}

/*
 * Function to start UDP connections
 *
 * Char in[] takes char array of desired port for inbound communications
 *
 * Int out takes an integer respresentation of the socket on the other client
 */
void PPP::start_com(char in[], char out[]){
    recv_sock = updSocket(in);
    m_send_port = out;

    // For testing
    cout << "Port Number Recevied: " << in << endl;
}

/*
 * Function to put in receive client mode
 * 
 * Parameter is socket number of the server
 */
void* PPP::msg_recv(void* arg)
{
    PPP* ppp = (PPP*) arg;
    char msg_buf[1024];
    struct sockaddr_in cliaddr;
    socklen_t len;
    int n;

    // For testing
    cout << "Message receive thread running" << endl;

    while (1) {
        memset(&msg_buf, 0, sizeof(msg_buf));
        len = sizeof(cliaddr);

        // For testing
        cout << "Waiting for message" << endl;

        // Read message from socket
        n = recvfrom(ppp->recv_sock, msg_buf, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        Message* msg = new Message(msg_buf, n);

        // For testing
        cout << "size of mesage received on socket: " << n << endl;

        // Strip headers
        eth_header* stripped = (eth_header*)msg->msgStripHdr(sizeof(eth_header));
        int protocol_id = stripped->hlp;

        // For testing - Header stripping works here?!
        cout << "Protocol ID stripped in eth header" << protocol_id << endl;
        cout << "Message stripped in eth header" << stripped->m_size << endl;

        // Build pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 0;
        send_pipe.msg = msg;
        
        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->eth_recv_pipe.pipe_mutex);

        // Write to pipe
        write(ppp->eth_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // For testing
        cout << "Socket message sent on pipe for processing" << endl;

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->eth_recv_pipe.pipe_mutex);
    }
}

void PPP::msg_send(Message* msg, int protocol_id){
    // For testing
    char* test = new char[1024];
    msg->msgFlat(test);
    cout << "Message received for sending: " << test << endl;

    if (protocol_id == 5) {
        pipe_unit ftp;
        ftp.protocol_id = 5;
        ftp.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ftp_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(ftp_send_pipe.pipe_d[1], (char*) &ftp, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ftp_send_pipe.pipe_mutex);
    }

    else if (protocol_id == 6){
        pipe_unit tel;
        tel.protocol_id = 6;
        tel.msg = msg;
        
        // Acquire mutex lock on pipe
        pthread_mutex_lock(tel_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(tel_send_pipe.pipe_d[1], (char*) &tel, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(tel_send_pipe.pipe_mutex);
    }

    else if (protocol_id == 7){
        pipe_unit rdp;
        rdp.protocol_id = 7;
        rdp.msg = msg;
        
        // Acquire mutex lock on pipe
        pthread_mutex_lock(rdp_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(rdp_send_pipe.pipe_d[1], (char*) &rdp, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(rdp_send_pipe.pipe_mutex);
    }

    else if (protocol_id == 8){
        pipe_unit dns;
        dns.protocol_id = 8;
        dns.msg = msg;

        // For testing
        char* test2 = new char[1024];
        dns.msg->msgFlat(test2);
        cout << "Message in pipe_unit struct for dns: " << test2 << endl;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(dns_send_pipe.pipe_mutex);

        // Write to ftp send pipe
        write(dns_send_pipe.pipe_d[1], (char*) &dns, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(dns_send_pipe.pipe_mutex);
    }

    else{
        cout << "Invalid protocol_id" << endl;
    }
}

void* PPP::eth_send(void* arg){
    PPP* ppp = (PPP*) arg;
    struct sockaddr_in servaddr;
    socklen_t len;
    struct hostent *phe;    // pointer to host information entry

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    char* host = "localhost";

    // Map port number (char string) to port number (int)
    if ((servaddr.sin_port = htons((unsigned short)atoi(ppp->m_send_port))) == 0)
        errexit("can't get \"%s\" port number\n", ppp->m_send_port);

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
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait until read succeeds
        read(ppp->eth_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Store message in variable
        msg = read_pipe->msg;

        // Create new header
        eth_header* h = (eth_header *) malloc( sizeof(eth_header));
        h->hlp = read_pipe->protocol_id;
        h->m_size = msg->msgLen();

        // For testing
        cout << "HLP in eth send: " << h->hlp << endl;
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(eth_header));

        // Flaten message to buffer
        char msg_buf[1024];
        memset(&msg_buf, 0, sizeof(msg_buf));
        msg->msgFlat(msg_buf);

        // Sent message to network
        if (sendto(upd_sock, msg_buf, msg->msgLen(), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        printf("Error with sendto %s\n", strerror(errno));

        // For testing
        cout << "Message sent over network to peer" << endl;
    }


}

void* PPP::eth_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait to read message from pipe
        read(ppp->eth_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // For testing
        cout << "Message read from receive pipe in eth recv" << endl;

        // Strip headers
        msg = read_pipe->msg;
        eth_header* stripped = (eth_header*)msg->msgStripHdr(sizeof(eth_header));
        int protocol_id = stripped->hlp;

        // For testing
        cout << "Protocol ID stripped in eth header" << protocol_id << endl;
        cout << "Message stripped in eth header" << stripped->m_size << endl;

        // Create new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 1;
        send_pipe.msg = msg;

        if (protocol_id == 2){
            // Lock mutex
            pthread_mutex_lock(ppp->ip_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->ip_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->ip_recv_pipe.pipe_mutex);
        }
        else{
            cout << "Error invalid protocol going up from eth" << endl;
        }
    }
}

void* PPP::IP_send(void* arg){
    PPP* ppp = (PPP*) arg;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait for message to send
        read(ppp->ip_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Store message in variable
        msg = read_pipe->msg;

        // For testing
        //UDP_header* h2 = (UDP_header*)msg->msgStripHdr(sizeof(UDP_header));
        //cout << "Size in stripped header for UDP: " << h2->m_size << endl;
        //cout << "HLP in stripped header for UDP: " << h2->hlp << endl;

        // Create new header
        IP_header* h = (IP_header *) malloc( sizeof(IP_header));
        h->hlp = read_pipe->protocol_id;
        h->m_size = msg->msgLen();
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(IP_header));

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 2;
        send_pipe.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->eth_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->eth_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->eth_send_pipe.pipe_mutex);
    }
}

void* PPP::IP_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->ip_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;// Create new header
        DNS_header* h = (DNS_header *) malloc( sizeof(DNS_header));
        h->m_size = msg->msgLen();

        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(DNS_header));
        IP_header* stripped = (IP_header*)msg->msgStripHdr(sizeof(IP_header));
        int protocol_id = stripped->hlp;

        // Strip headers
        //msg = read_pipe->msg;
        //int protocol_id = atoi(msg->msgStripHdr(2));
        //msg->msgStripHdr(14);

        // Create new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 2;
        send_pipe.msg = msg;

        if (protocol_id == 3){
            // Lock mutex
            pthread_mutex_lock(ppp->tcp_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->tcp_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->tcp_recv_pipe.pipe_mutex);
        }
        else if (protocol_id == 4){
            // Lock mutex
            pthread_mutex_lock(ppp->udp_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->udp_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->udp_recv_pipe.pipe_mutex);
        }
        else{
            cout << "Error invalid protocol going up from ip" << endl;
        }
    }
}

void* PPP::TCP_send(void* arg){
    PPP* ppp = (PPP*) arg;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait for message to send
        read(ppp->tcp_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Store message in variable
        msg = read_pipe->msg;

        // Create new header
        TCP_header* h = (TCP_header *) malloc( sizeof(TCP_header));
        h->hlp = read_pipe->protocol_id;
        h->m_size = msg->msgLen();
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(TCP_header));

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 3;
        send_pipe.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->ip_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->ip_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->ip_send_pipe.pipe_mutex);
    }
}

void* PPP::TCP_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->tcp_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;
        TCP_header* stripped = (TCP_header*)msg->msgStripHdr(sizeof(TCP_header));
        int protocol_id = stripped->hlp;

        // Strip headers
        //msg = read_pipe->msg;
        //int protocol_id = atoi(msg->msgStripHdr(2));
        //msg->msgStripHdr(6);

        // Create new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 3;
        send_pipe.msg = msg;

        if (protocol_id == 5){
            // Lock mutex
            pthread_mutex_lock(ppp->ftp_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->ftp_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->ftp_recv_pipe.pipe_mutex);
        }
        else if (protocol_id == 6){
            // Lock mutex
            pthread_mutex_lock(ppp->tel_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->tel_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->tel_recv_pipe.pipe_mutex);
        }
        else{
            cout << "Error invalid protocol going up from tcp" << endl;
        }
    }
}

void* PPP::UDP_send(void* arg){
    PPP* ppp = (PPP*) arg;

    // For testing
    cout << "Waiting for UDP message to send" << endl;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait for message to send
        read(ppp->udp_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // For testing
        cout << "UDP message received to send" << endl;

        // Store message in variable
        msg = read_pipe->msg;

        // For testing
        //DNS_header* h2 = (DNS_header*)msg->msgStripHdr(sizeof(DNS_header));
        //cout << "Size in stripped header: " << h2->m_size << endl;

        // Create new header
        UDP_header* h = (UDP_header *) malloc( sizeof(UDP_header));
        h->hlp = read_pipe->protocol_id;
        h->m_size = msg->msgLen();

        // For testing
        //cout << "Protocol ID passed to UDP send: " << h->hlp << endl;
        //cout << "HLP in stripped header for pass to UDP send: " << h2->hlp << endl;
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(UDP_header));

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 4;
        send_pipe.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->ip_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->ip_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // For testing
        cout << "UDP message written to IP" << endl;

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->ip_send_pipe.pipe_mutex);
    }
}

void* PPP::UDP_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->udp_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;
        UDP_header* stripped = (UDP_header*)msg->msgStripHdr(sizeof(UDP_header));
        int protocol_id = stripped->hlp;

        // Strip headers
        //msg = read_pipe->msg;
        //int protocol_id = atoi(msg->msgStripHdr(2));
        //msg->msgStripHdr(6);

        // Create new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 4;
        send_pipe.msg = msg;

        if (protocol_id == 7){
            // Lock mutex
            pthread_mutex_lock(ppp->rdp_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->rdp_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->rdp_recv_pipe.pipe_mutex);
        }
        else if (protocol_id == 8){
            // Lock mutex
            pthread_mutex_lock(ppp->dns_recv_pipe.pipe_mutex);

            // Write to pipe
            write(ppp->dns_recv_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

            // Unlock mutex
            pthread_mutex_unlock(ppp->dns_recv_pipe.pipe_mutex);
        }
        else{
            cout << "Error invalid protocol going up from udp" << endl;
        }
    }
}

void* PPP::FTP_send(void* arg){
    PPP* ppp = (PPP*) arg;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait for message to send
        read(ppp->ftp_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Store message in variable
        msg = read_pipe->msg;

        // Create new header
        ftp_header* h = (ftp_header *) malloc( sizeof(ftp_header));
        h->m_size = msg->msgLen();
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(ftp_header));

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 5;
        send_pipe.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->tcp_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->tcp_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->tcp_send_pipe.pipe_mutex);
    }
}

void* PPP::FTP_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->ftp_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;
        msg->msgStripHdr(sizeof(ftp_header));

        // Copy message to buffer and terminate line
        char* msg_buf = new char[1024];
        msg->msgFlat(msg_buf);
        msg_buf[msg->msgLen()] = '\n';

        // Print to user
        cout << "FTP Message Recevied: " << msg_buf;

        // Clean up
        delete msg_buf;
    }
}

void* PPP::tel_send(void* arg){
        PPP* ppp = (PPP*) arg;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait for message to send
        read(ppp->tel_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Store message in variable
        msg = read_pipe->msg;

        // Create new header
        tel_header* h = (tel_header*) malloc( sizeof(tel_header));
        h->m_size = msg->msgLen();
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(tel_header));

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 6;
        send_pipe.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->tcp_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->tcp_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->tcp_send_pipe.pipe_mutex);
    }
}

void* PPP::tel_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->tel_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;
        msg->msgStripHdr(sizeof(tel_header));

        // Copy message to buffer and terminate line
        char* msg_buf = new char[1024];
        msg->msgFlat(msg_buf);
        msg_buf[msg->msgLen()] = '\n';

        // Print to user
        cout << "Telnet Message Recevied: " << msg_buf;

        // Clean up
        delete msg_buf;
    }
}

void* PPP::RDP_send(void* arg){
        PPP* ppp = (PPP*) arg;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // Wait for message to send
        read(ppp->rdp_send_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Store message in variable
        msg = read_pipe->msg;

        // Create new header
        RDP_header* h = (RDP_header *) malloc( sizeof(RDP_header));
        h->m_size = msg->msgLen();
        
        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(RDP_header));

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 7;
        send_pipe.msg = msg;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->udp_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->udp_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->udp_send_pipe.pipe_mutex);
    }
}

void* PPP::RDP_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->rdp_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;
        msg->msgStripHdr(sizeof(RDP_header));

        // Copy message to buffer and terminate line
        char* msg_buf = new char[1024];
        msg->msgFlat(msg_buf);
        msg_buf[msg->msgLen()] = '\n';

        // Print to user
        cout << "RDP Message Recevied: " << msg_buf;

        // Clean up
        delete msg_buf;
    }
}

void* PPP::DNS_send(void* arg){
        PPP* ppp = (PPP*) arg;

    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        // For testing
        cout << "Waiting for DNS message to send" << endl;

        // Wait for message to send
        read(ppp->dns_send_pipe.pipe_d[0], (char*)read_pipe, sizeof(pipe_unit));

        // For testing
        cout << "Protocol ID test: " << read_pipe->protocol_id << endl;

        // Store message in variable
        msg = read_pipe->msg;

        // For testing
        char* test = new char[1024];
        msg->msgFlat(test);
        cout << "Message received for sending in dns_send: " << test << endl;

        // Create new header
        DNS_header* h = (DNS_header *) malloc( sizeof(DNS_header));
        h->m_size = msg->msgLen();

        // Add header to message
        msg->msgAddHdr((char*) h, sizeof(DNS_header));
        
        // For testing
        //DNS_header* h2 = (DNS_header*)msg->msgStripHdr(sizeof(DNS_header));
        cout << "Size after adding DNS_header: " << msg->msgLen() << endl;

        // Build new pipe unit
        pipe_unit send_pipe;
        send_pipe.protocol_id = 8;
        send_pipe.msg = msg;

        // For testing
        cout << "DNS pipe unit built" << endl;

        // Acquire mutex lock on pipe
        pthread_mutex_lock(ppp->udp_send_pipe.pipe_mutex);

        // Write to eth send pipe
        write(ppp->udp_send_pipe.pipe_d[1], (char*) &send_pipe, sizeof(pipe_unit));

        // Remove mutex lock on pipe
        pthread_mutex_unlock(ppp->udp_send_pipe.pipe_mutex);
    }
}

void* PPP::DNS_recv(void* arg){
    PPP* ppp = (PPP*) arg;
    
    while(1){
        Message* msg;
        pipe_unit* read_pipe = new pipe_unit;

        read(ppp->dns_recv_pipe.pipe_d[0], (char*) read_pipe, sizeof(pipe_unit));

        // Strip headers
        msg = read_pipe->msg;
        msg->msgStripHdr(sizeof(DNS_header));

        // Copy message to buffer and terminate line
        char* msg_buf = new char[1024];
        msg->msgFlat(msg_buf);
        msg_buf[msg->msgLen()] = '\n';

        // Print to user
        cout << "DNS Message Recevied: " << msg_buf;

        // Clean up
        delete msg_buf;
    }
}
