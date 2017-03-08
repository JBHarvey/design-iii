#include <stdio.h>
#include <netinet/in.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/ioctl.h>


#include "network.h"

#define BUFFER_SIZE 1024

static uint32_t total_packet_length;
static uint8_t *packet;
static uint32_t received_packet_length;
static int newest_sd = -1;

struct Packet {
    struct Packet *next;
    unsigned int length;
    unsigned int sent;
    uint8_t data[];
};

static struct Packet *first;
static struct Packet *last;

static void clearFirstPacket()
{
    struct Packet *next = first->next;
    free(first);

    if(first == last) {
        last = 0;
    }

    first = next;
}

static void sendDataSocket(int fd)
{
    while(first) {
        uint8_t *buf = first->data + first->sent;
        unsigned int length = first->length - first->sent;

        int ret = send(fd, buf, length, MSG_NOSIGNAL);
        printf("sent %i\n", ret);

        if(ret <= 0) {
            break;
        } else {
            if(ret == length) {
                clearFirstPacket();
            } else if(ret < length) {
                first->sent += ret;
                break;
            }
        }
    }
}

void addPacket(uint8_t *data, uint32_t length)
{
    struct Packet *new_packet = malloc(sizeof(struct Packet) + sizeof(uint32_t) + length);
    printf("add %i\n", length);
    new_packet->next = 0;
    new_packet->length = length + sizeof(uint32_t);
    new_packet->sent = 0;
    memcpy(new_packet->data, &length, sizeof(uint32_t));
    memcpy(new_packet->data + sizeof(uint32_t), data, length);

    if(last) {
        last->next = new_packet;
    } else {
        first = new_packet;
    }

    last = new_packet;
}

/* Accept client requests */
void acceptCallback(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sd;
    struct ev_io *w_client = (struct ev_io *) malloc(sizeof(struct ev_io));

    if(EV_ERROR & revents) {
        perror("got invalid event");
        return;
    }

    // Accept client request
    client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

    if(client_sd < 0) {
        perror("accept error");
        return;
    }

    total_packet_length = received_packet_length = 0;

    /* makes disrupting comms easy for bad actor */
    {
        newest_sd = client_sd;

        if(packet) {
            free(packet);
            packet = 0;
        }

        if(first && first->sent) {
            clearFirstPacket();
        }
    }

    printf("Successfully connected with client.\n");

    // Initialize and start watcher to read client requests
    ev_io_init(w_client, readWriteCallback, client_sd, EV_READ | EV_WRITE);
    ev_io_start(loop, w_client);
}

/* Packet format [uint32 length][uint8 id] */

/* Read client message */
void readWriteCallback(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    char buffer[BUFFER_SIZE];
    ssize_t read;

    if(EV_ERROR & revents) {
        perror("got invalid event");
        return;
    }

    if(newest_sd > 0 && watcher->fd != newest_sd) {
        printf("error, old connection still live, closing\n");
        ev_io_stop(loop, watcher);
        free(watcher);
        return;
    }

    if(EV_WRITE & revents) {
        sendDataSocket(watcher->fd);
    }

    if(!(EV_READ & revents)) {
        return;
    }

    if(!packet) {
        int bytes_available = 0;
        ioctl(watcher->fd, FIONREAD, &bytes_available);

        /* make sure packet beggining can always be read at once */
        if(bytes_available < sizeof(total_packet_length)) {
            return;
        }

        read = recv(watcher->fd, buffer, sizeof(total_packet_length), 0);
    } else {
        unsigned int read_size = 0;

        if(BUFFER_SIZE > (total_packet_length - received_packet_length)) {
            read_size = (total_packet_length - received_packet_length);
        } else {
            read_size = BUFFER_SIZE;
        }

        read = recv(watcher->fd, buffer, read_size, 0);
    }

    // Receive message from client socket


    if(read < 0) {
        perror("read error");
        return;
    }

    if(read == 0) {
        // Stop and free watcher if client socket is closing
        ev_io_stop(loop, watcher);
        free(watcher);
        perror("peer might closing");
        return;
    } else {
        if(packet) {
            //printf("%i %u\n", read, received_packet_length);
            memcpy(packet + received_packet_length, buffer, read);
            received_packet_length += read;

            if(total_packet_length  == received_packet_length) {
                //CALLBACK
                handleReceivedPacket(packet, received_packet_length);
                //printf("Message of length %u:%s\n", received_packet_length, packet);
                //
                free(packet);
                packet = 0;
                total_packet_length = received_packet_length = 0;
            }
        } else if(read == 4) {
            memcpy(&total_packet_length, buffer, sizeof(total_packet_length));
            packet = malloc(total_packet_length);

            if(!packet) {
                perror("malloc error");
                return;
            }
        } else {
            printf("shit packet error\n");
        }
    }
}

