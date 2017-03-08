#include <stdio.h>
#include <netinet/in.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>


#define PORT_NO 3033

#include "protocol.h"
#include "ev_init.h"

int main()
{
    struct Communication *communication = communication_initServer(PORT_NO);

    // Start infinite loop
    while(1) {
        sendContinuePacket();
        communication_do(communication, 1);
    }

    return 0;
}

