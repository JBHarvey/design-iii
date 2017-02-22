#include <stdio.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void add_packet(uint8_t *data, uint32_t length);

void handle_recv_packet(uint8_t *data, uint32_t length);
