#ifndef _PPP_
#define _PPP_

#include <map>
#include <semaphore.h>
#include <string.h>
#include <mutex>
#include <iostream>
#include "message.h"
#include "threadpool.h"

typedef void (*function_pointer)(void*);

class PPP
{
public:
    PPP();
    ~PPP();
    
    void msg_send(Message* msg, int protocol_id);
    void msg_recv(int sock_num);

private:
    ThreadPool* m_thread_pool;

    static void* eth_send(void* arg);
    static void* eth_recv(void* arg);
    static void* IP_send(void* arg);
    static void* IP_recv(void* arg);
    static void* TCP_send(void* arg);
    static void* TCP_recv(void* arg);
    static void* UDP_send(void* arg);
    static void* UDP_recv(void* arg);
    static void* FTP_send(void* arg);
    static void* FTP_recv(void* arg);
    static void* tel_send(void* arg);
    static void* tel_recv(void* arg);
    static void* RDP_send(void* arg);
    static void* RDP_recv(void* arg);
    static void* DNS_send(void* arg);
    static void* DNS_recv(void* arg);

    // Allocate arrays for pipes
    int ftp_to_tcp[2], tcp_to_ftp[2], app_to_ftp[2];
    int tel_to_tcp[2], tcp_to_tel[2], app_to_tel[2];
    int rdp_to_udp[2], udp_to_rdp[2], app_to_rdp[2];
    int dns_to_udp[2], udp_to_dns[2], app_to_dns[2];
    int tcp_to_ip[2], ip_to_tcp[2], udp_to_ip[2];
    int ip_to_udp[2], ip_to_eth[2], eth_to_ip[2];
    int net_to_eth[2];

    // Variable for storing the udp socket for the virtual network
    int udp_sock;
};

#endif