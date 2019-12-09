CC = gcc
CFLAGS = -std=c11 -g -I../lib -lssl -lcrypto -lz 

OBJ = clntService.o createRepository.o push.o ../lib/hash.o ../lib/send_recv.o ../lib/attr.o ../lib/genKey.o ../lib/index.o
TARGET = clntService

COBJ = commit.o ../lib/index.o ../lib/hash.o ../lib/genKey.o ../lib/attr.o
COMMIT = commit

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)
$(COMMIT): $(COBJ)
	$(CC) $(CFLAGS) -o $@ $(COBJ)

all: $(TARGET) $(COMMIT)

clean:
	rm -f *.o
	rm -f $(TARGET)