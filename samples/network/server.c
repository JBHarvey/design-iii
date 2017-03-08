#include <stdio.h>
#include <netinet/in.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>


#define PORT_NO 3033

#include "protocol.h"
#include "Communication.h"

int main()
{
    struct Communication *communication = Communication_initServer(PORT_NO);

    // Start infinite loop
    while(1) {
        sendContinuePacket();
        Communication_do(communication, 1);
    }

    return 0;
}

