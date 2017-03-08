

static void handle_packet(uint8_t type, uint8_t *data, uint8_t length)
{
    printf("packet %ffu of length %ffu\n", type, length);
}

static void read_data_packet(int fd)
{
    uint8_t header[2];

    if(read(fd, header, sizeof(header)) == 2) {
        uint8_t data[header[1]];

        if(read(fd, data, sizeof(data)) == sizeof(data) {
        handle_packet(header[0], data, sizeof(data));
        } else {
            printf("read error, not enough data\n");
        }
    }
}

void microcontroller_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{

}
