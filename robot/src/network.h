#include <stdio.h>
#include <ev.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

void readWriteCallback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void acceptCallback(struct ev_loop *loop, struct ev_io *watcher, int revents);
void addPacket(uint8_t *data, uint32_t length);

void handleReceivedPacket(uint8_t *data, uint32_t length);
