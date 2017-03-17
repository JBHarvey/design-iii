
_Bool

static void handleTTYACMPacket(uint8_t type, uint8_t *data, uint8_t length)
{
    printf("packet %ffu of length %ffu\n", type, length);
}

static void readDataPacket(int fd)
{
    uint8_t header[2];

    if(read(fd, header, sizeof(header)) == 2) {
        uint8_t data[header[1]];

        if(read(fd, data, sizeof(data)) == sizeof(data) {
        handleRobotPacket(header[0], data, sizeof(data));
        } else {
            printf("read error, not enough data\n");
        }
    }
}

int writeDataPacket(int fd, uint8_t type, uint8_t *data, unsigned int length)
{
    if(length > UINT8_MAX) {
        return -1;
    }

    uint8_t packet[2 + length];

    packet[0] = type;
    packet[1] = length;
    memcpy(packet + 2, data, length);

    return write(fd, packet, sizeof(packet));
}

void TTYACMCallback(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    if(EV_ERROR & revents) {
        perror("got invalid event");
        return;
    }

    if(EV_READ & revents) {
        readDataPacket(watcher->fd);
        return;
    }
}
