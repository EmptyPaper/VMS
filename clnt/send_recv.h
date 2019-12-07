#define SERV_IP "127.0.0.1"
#define SERV_PORT 33333

#define CREATE_REPO 0b00000001
#define PUSH_REQ 0b000000010
#define PULL_REQ 0b00000100
#define LOG_REQ 0b00001000


void _send(int sock, void* item, int size);
void _recv(int sock, void* item, int size);



