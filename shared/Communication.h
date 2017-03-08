
struct Communication {
    struct ev_loop *loop;
};

struct Communication *Communication_initClient(char *ip, unsigned short port);
struct Communication *Communication_initServer(unsigned short port);

void Communication_do(struct Communication *communication, unsigned int milliseconds);
void Communication_close(struct Communication *communication);
