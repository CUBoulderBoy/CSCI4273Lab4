#include <iostream>
#include <stdio.h>
#include "ppm.h"
#include "message.h"
#include "protocolstructs.h"
#include <unistd.h>

using namespace std;

int main(){
    PPM host_a("32000", "32001");
    PPM host_b("32001", "32000");

    sleep(1);

    char* text = "This is a test";
    Message* msg1 = new Message(text, 14);
    Message* msg2 = new Message(text, 14);
    Message* msg3 = new Message(text, 14);
    Message* msg4 = new Message(text, 14);

    host_a.FTP_send(FTP_ID, msg1);
    host_a.telnet_send(TELNET_ID, msg2);
    host_a.RDP_send(RDP_ID, msg3);
    host_a.DNS_send(DNS_ID, msg4);

    while(1){
        continue;
    }
}