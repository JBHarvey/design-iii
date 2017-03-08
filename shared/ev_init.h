
struct Communication {
    struct ev_loop *loop;
};

struct Communication *communication_initClient(char *ip, unsigned short port);
struct Communication *communication_initServer(unsigned short port);

void communication_do(struct Communication *communication, unsigned int milliseconds);
void communication_close(struct Communication *communication);
