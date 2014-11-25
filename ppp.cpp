#include "ppp.h"
#include "updsocket.cpp"

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

using namespace std;

struct ppp_pipe{
    int in_pipe;
    int out_pipe;
    int out_pipe2;
};

PPM::PPM()
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

    // Create thread pool
    m_thread_pool = new ThreadPool(22);

    // Create thread ids
    pthread_t tid_1, tid_2, tid_3, tid_4, tid_5, tid_6, tid_7, tid_8, tid_9;
    pthread_t tid_10, tid_11, tid_12, tid_13, tid_14, tid_15, tid_16, tid_17;
    pthread_t tid_18, tid_19, tid_20, tid_21, tid_22;

    // Create vars for threads
    int err;

    // start a thread for ethernet up
    ppp_pipe eth_up;
    eth_up.in_pipe = net_in;
    eth_up.out_pipe = eth_to_ip[1];
    err = pthread_create(&tid_1, NULL, PPP::eth_recv, (void*) eth_up);
    if (err != 0) {
        cout << "eth_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ethernet down
    ppp_pipe eth_dn;
    eth_dn.in_pipe = ip_to_eth[0];
    eth_dn.out_pipe = net_out;
    err = pthread_create(&tid_2, NULL, PPP::eth_send, (void*) eth_dn);
    if (err != 0) {
        cout << "eth_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ip to udp and tcp
    ppp_pipe ip_up;
    ip_up.in_pipe = eth_to_ip[0];
    ip_up.out_pipe = ip_to_udp[1];
    ip_up.out_pipe2 = ip_to_tcp[1];
    err = pthread_create(&tid_3, NULL, PPP::IP_recv, (void*) ip_up);
    if (err != 0) {
        cout << "IP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for udp down to ip
    ppp_pipe ip_dn_udp;
    ip_dn_udp.in_pipe = udp_to_ip[0];
    ip_dn_udp.out_pipe = ip_to_eth[1];
    err = pthread_create(&tid_4, NULL, PPP::IP_send, (void*) ip_dn_udp);
    if (err != 0) {
        cout << "IP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for tcp down to ip
    ppp_pipe ip_dn_tcp;
    ip_dn_tcp.in_pipe = tcp_to_ip[0];
    ip_dn_tcp.out_pipe = ip_to_eth[1];
    err = pthread_create(&tid_5, NULL, PPP::IP_send, (void*) ip_dn_tcp);
    if (err != 0) {
        cout << "IP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for udp to rdp and dns
    ppp_pipe udp_up;
    udp_up.in_pipe = ip_to_udp[0];
    udp_up.out_pipe = udp_to_rdp[1];
    udp_up.out_pipe2 = udp_to_dns[1];
    err = pthread_create(&tid_6, NULL, PPP::UDP_recv, (void*) udp_up);
    if (err != 0) {
        cout << "UDP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for rdp down to udp
    ppp_pipe rdp_dn_udp;
    rdp_dn_udp.in_pipe = rdp_to_udp[0];
    rdp_dn_udp.out_pipe = udp_to_ip[1];
    err = pthread_create(&tid_7, NULL, PPP::UDP_send, (void*) rdp_dn_udp);
    if (err != 0) {
        cout << "UDP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for dns down to udp
    ppp_pipe dns_dn_udp;
    dns_dn_udp.in_pipe = dns_to_udp[0];
    dns_dn_udp.out_pipe = udp_to_ip[1];
    err = pthread_create(&tid_8, NULL, PPP::UDP_send, (void*) dns_dn_udp);
    if (err != 0) {
        cout << "UDP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for rdp print
    ppp_pipe rdp_print;
    rdp_print.in_pipe = udp_to_rdp[0];
    err = pthread_create(&tid_9, NULL, PPP::RDP_recv, (void*) rdp_print);
    if (err != 0) {
        cout << "RDP_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for dns print
    ppp_pipe dns_print;
    dns_print.in_pipe = udp_to_rdp[0];
    err = pthread_create(&tid_10, NULL, PPP::DNS_recv, (void*) dns_print);
    if (err != 0) {
        cout << "DNS_recv() failed: " << err << endl;
        exit(1);
    }

    // start a thread for tcp to ftp and tel
    ppp_pipe tcp_up;
    tcp_up.in_pipe = ip_to_tcp[0];
    tcp_up.out_pipe = tcp_to_ftp[1];
    tcp_up.out_pipe2 = tcp_to_tel[1];
    err = pthread_create(&tid_11, NULL, PPP::TCP_recv, (void*) tcp_up);
    if (err != 0) {
        cout << "TCP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for ftp down to tcp
    ppp_pipe ftp_dn_tcp;
    ftp_dn_tcp.in_pipe = ftp_to_tcp[0];
    ftp_dn_tcp.out_pipe = udp_to_ip[1];
    err = pthread_create(&tid_12, NULL, PPP::TCP_send, (void*) ftp_dn_tcp);
    if (err != 0) {
        cout << "TCP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for dns down to udp
    ppp_pipe tel_dn_tcp;
    tel_dn_tcp.in_pipe = dns_to_udp[0];
    tel_dn_tcp.out_pipe = udp_to_ip[1];
    err = pthread_create(&tid_13, NULL, PPP::UDP_send, (void*) tel_dn_tcp);
    if (err != 0) {
        cout << "UDP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for rdp print
    ppp_pipe rdp_print;
    rdp_print.in_pipe = udp_to_rdp[0];
    err = pthread_create(&tid_9, NULL, PPP::RDP_send, (void*) rdp_print);
    if (err != 0) {
        cout << "RDP_send() failed: " << err << endl;
        exit(1);
    }

    // start a thread for dns print
    ppp_pipe dns_print;
    dns_print.in_pipe = udp_to_rdp[0];
    err = pthread_create(&tid_10, NULL, PPP::DNS_send, (void*) dns_print);
    if (err != 0) {
        cout << "DNS_send() failed: " << err << endl;
        exit(1);
    }

}

PPP::~PPP()
{
    delete m_thread_pool;
}

void PPP::msg_recv(void* arg)
{
    PPP* ppp = (PPP*) arg;
    char udp_portnum[6] = "32001";
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

void PPP::msg_send(Message* msg, int protocol_id){
    ;
}

void PPP::eth_send(void* arg){
    ;
}

void PPP::eth_recv(void* arg){
    ;
}

void PPP::IP_send(void* arg){
    ;
}

void PPP::IP_recv(void* arg){
    ;
}

void PPP::TCP_send(void* arg){
    ;
}

void PPP::TCP_recv(void* arg){
    ;
}

void PPP::UDP_send(void* arg){
    ;
}

void PPP::UDP_recv(void* arg){
    ;
}

void PPP::FTP_send(void* arg){
    ;
}

void PPP::FTP_recv(void* arg){
    ;
}

void PPP::tel_send(void* arg){
    ;
}

void PPP::tel_recv(void* arg){
    ;
}

void PPP::RDP_send(void* arg){
    ;
}

void PPP::RDP_recv(void* arg){
    ;
}

void PPP::DNS_send(void* arg){
    ;
}

void PPP::DNS_recv(void* arg){
    ;
}
