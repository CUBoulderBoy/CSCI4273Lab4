#include <iostream>
#include <stdio.h>
#include "ppm.h"
#include "message.h"
#include <unistd.h>

using namespace std;

int main(){
    PPM host_a("32000", "32001");
    PPM host_b("32001", "32000");
    // PPM host_b("32001", 32002);

    sleep(1);

    char* text = "This is a test";
    Message* msg = new Message(text, 14);
    host_a.ethernet_send(8, msg);

    while(1){
        continue;
    }
}