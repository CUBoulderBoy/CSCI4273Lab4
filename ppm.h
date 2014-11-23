#ifndef _PPM_
#define _PPM_

#include <map>
#include <semaphore.h>
#include <string.h>
#include <mutex>
#include <iostream>
#include "message.h"
#include "ThreadPool.h"

typedef void (*function_pointer)(void*);

class PPM
{
public:
    PPM();
    ~PPM();
    void ethernet_send(int protocol_id, Message* msg);
    void ethernet_recv(Message* msg);
    void IP_send(int protocol_id, Message* msg);
    void IP_recv(Message* msg);
    void TCP_send(int protocol_id, Message* msg);
    void TCP_recv(Message* msg);
    void UDP_send(int protocol_id, Message* msg);
    void UDP_recv(Message* msg);
    void FTP_send(int protocol_id, Message* msg);
    void FTP_recv(Message* msg);
    void telnet_send(int protocol_id, Message* msg);
    void telnet_recv(Message* msg);
    void RDP_send(int protocol_id, Message* msg);
    void RDP_recv(Message* msg);
    void DNS_send(int protocol_id, Message* msg);
    void DNS_recv(Message* msg);

private:
    ThreadPool* m_thread_pool;
    void* read_upd(void* arg);


};

#endif
