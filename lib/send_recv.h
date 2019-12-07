#define SERV_IP "127.0.0.1"
#define SERV_PORT 33333

#define CREATE_REPO 0b00000001
#define PUSH_REQ 0b000000010
#define PULL_REQ 0b00000100
#define LOG_REQ 0b00001000

#define SIGBUF 64


void _send(int sock, void* item, int size);
int _recv(int sock, void* item, int size);

void createRepository(int sock);
void push(int sock);
void pull(int sock);
void Log(int sock);


