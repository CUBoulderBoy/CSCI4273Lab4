#include "ppp.h"
#include "updsocket.cpp"

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

using namespace std;

PPP::PPP()
{
    // Create pipes for threads
    pipe(ftp_to_tcp);
    pipe(tcp_to_ftp);
    pipe(app_to_ftp);
    pipe(tel_to_tcp);
    pipe(tcp_to_tel);
    pipe(app_to_tel);
    pipe(rdp_to_udp);
    pipe(udp_to_rdp);
    pipe(app_to_rdp);
    pipe(dns_to_udp);
    pipe(udp_to_dns);
    pipe(app_to_dns);
    pipe(tcp_to_ip);
    pipe(ip_to_tcp);
    pipe(udp_to_ip);
    pipe(ip_to_udp);
    pipe(ip_to_eth);
    pipe(eth_to_ip);
    pipe(net_to_eth);

    // Create thread pool
    m_thread_pool = new ThreadPool(16);

    // Create thread ids
    pthread_t tid_1, tid_2, tid_3, tid_4, tid_5, tid_6, tid_7, tid_8, tid_9;
    pthread_t tid_10, tid_11, tid_12, tid_13, tid_14, tid_15, tid_16;

    // Create vars for threads
    int err;

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
    err = pthread_create(&tid_15, NULL, PPP::DNS_recv, NULL);
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

}

PPP::~PPP()
{
    delete m_thread_pool;
}

void PPP::msg_recv(int sock_num)
{

    char udp_portnum[6] = "32001";
    int udp_sock = updSocket(udp_portnum);
    char mesg_buf[1024];
    struct sockaddr_in cliaddr;
    socklen_t len;
    int n;

    while (1) {
        memset(&mesg_buf, 0, sizeof(mesg_buf));
        len = sizeof(cliaddr);
        n = recvfrom(udp_sock, mesg_buf, 1024, 0, (struct sockaddr *)&cliaddr, &len);
        Message* msg = new Message(mesg_buf, n);
        write(net_to_eth[1], msg, sizeof(msg));
    }
}

void PPP::msg_send(Message* msg, int protocol_id){
    if (protocol_id == 5) {
        write(app_to_ftp[1], msg, sizeof(msg));
    }

    else if (protocol_id == 6){
        write(app_to_tel[1], msg, sizeof(msg));
    }

    else if (protocol_id == 7){
        write(app_to_rdp[1], msg, sizeof(msg));
    }

    else if (protocol_id == 8){
        write(app_to_dns[1], msg, sizeof(msg));
    }

    else{
        cout << "Invalid protocol_id" << endl;
    }
}

void* PPP::eth_send(void* arg){
    PPP* ppp = (PPP*) arg;
    Message* msg;
    while(1){
        read(ppp->ip_to_eth[0], msg, sizeof(msg));
    }
}

void* PPP::eth_recv(void* arg){
    ;
}

void* PPP::IP_send(void* arg){
    ;
}

void* PPP::IP_recv(void* arg){
    ;
}

void* PPP::TCP_send(void* arg){
    ;
}

void* PPP::TCP_recv(void* arg){
    ;
}

void* PPP::UDP_send(void* arg){
    ;
}

void* PPP::UDP_recv(void* arg){
    ;
}

void* PPP::FTP_send(void* arg){
    ;
}

void* PPP::FTP_recv(void* arg){
    ;
}

void* PPP::tel_send(void* arg){
    ;
}

void* PPP::tel_recv(void* arg){
    ;
}

void* PPP::RDP_send(void* arg){
    ;
}

void* PPP::RDP_recv(void* arg){
    ;
}

void* PPP::DNS_send(void* arg){
    ;
}

void* PPP::DNS_recv(void* arg){
    ;
}
