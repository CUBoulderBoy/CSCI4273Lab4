#ifndef _PPM_
#define _PPM_

#include <map>
#include <semaphore.h>
#include <string.h>
#include <mutex>
#include <iostream>
#include "message.h"
#include "threadpool.h"

typedef void (*function_pointer)(void*);

class PPM
{
public:
    PPM();
    PPM(char* send_port, char* recv_port);
    ~PPM();
    
    void ethernet_send(int protocol_id, Message* msg);
    static void ethernet_recv(void* arg);
    void IP_send(int protocol_id, Message* msg);
    static void IP_recv(Message* msg);
    void TCP_send(int protocol_id, Message* msg);
    static void TCP_recv(Message* msg);
    void UDP_send(int protocol_id, Message* msg);
    static void UDP_recv(Message* msg);
    void FTP_send(int protocol_id, Message* msg);
    static void FTP_recv(Message* msg);
    void telnet_send(int protocol_id, Message* msg);
    static void telnet_recv(Message* msg);
    void RDP_send(int protocol_id, Message* msg);
    static void RDP_recv(Message* msg);
    void DNS_send(int protocol_id, Message* msg);
    static void DNS_recv(Message* msg);

    int m_num_sent = 0;
    int m_num_recv = 0;

private:
    ThreadPool* m_thread_pool;
    static void* read_upd(void* arg);
    char* m_send_port;
    char* m_recv_port;
};

#endif
