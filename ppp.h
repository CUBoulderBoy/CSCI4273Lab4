#ifndef _PPP_
#define _PPP_

#include "message.h"
#include "threadpool.h"

#include <mutex>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

typedef void (*function_pointer)(void*);

class PPP
{
public:
    // Struct for sending to pipes
    struct pipe_unit {
        int protocol_id;
        Message *msg;
    };

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Array for pipe descriptor
        int pipe_d[2];
     
        //Pointer to mutex
        pthread_mutex_t* pipe_mutex;
    } pipes;

    PPP(char in[], char out[]);
    ~PPP();
    
    void msg_send(Message* msg, int protocol_id);
    void start_com(char in[], char[]);

    // For tracking messages
    int m_num_sent = 0;
    int m_num_recv = 0;


    // Allocate structs for application level pipes
    pipes ftp_send_pipe, tel_send_pipe, rdp_send_pipe, dns_send_pipe;

    // Mutext locks for application level send pipes
    pthread_mutex_t ftp_send_mut;
    pthread_mutex_t tel_send_mut;
    pthread_mutex_t rdp_send_mut;
    pthread_mutex_t dns_send_mut;

private:
    ThreadPool* m_thread_pool;

    // Functions for protocols
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

    // Functions for receiving messages
    static void* msg_recv(void* arg);

    // Allocate structs for pipes
    pipes ftp_recv_pipe, tel_recv_pipe, rdp_recv_pipe, dns_recv_pipe;
    pipes tcp_send_pipe, tcp_recv_pipe, udp_send_pipe, udp_recv_pipe;
    pipes ip_send_pipe, ip_recv_pipe, eth_send_pipe, eth_recv_pipe;

    // Allocate mutex locks for pipes
    pthread_mutex_t ftp_recv_mut;
    pthread_mutex_t tel_recv_mut;
    pthread_mutex_t rdp_recv_mut;
    pthread_mutex_t dns_recv_mut;
    pthread_mutex_t tcp_send_mut;
    pthread_mutex_t tcp_recv_mut;
    pthread_mutex_t udp_send_mut;
    pthread_mutex_t udp_recv_mut;
    pthread_mutex_t ip_send_mut;
    pthread_mutex_t ip_recv_mut;
    pthread_mutex_t eth_send_mut;
    pthread_mutex_t eth_recv_mut;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[12];

        // Int for size of message without header
        int m_size;
    } IP_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[8];

        // Int for size of message without header
        int m_size;
    } eth_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[4];

        // Int for size of message without header
        int m_size;
    } TCP_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[4];

        // Int for size of message without header
        int m_size;
    } UDP_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[12];

        // Int for size of message without header
        int m_size;
    } RDP_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[8];

        // Int for size of message without header
        int m_size;
    } DNS_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[8];

        // Int for size of message without header
        int m_size;
    } ftp_header;

    //Struct to store arguements for the resquester threads
    typedef struct {
        // Int for higher level protocol
        int hlp;
     
        // Char array for other info
        char oi[8];

        // Int for size of message without header
        int m_size;
    } tel_header;

    // Variable for storing the udp socket for the virtual network
    int recv_sock, send_sock;

    char* m_send_port;
    char* m_recv_port;
};

#endif