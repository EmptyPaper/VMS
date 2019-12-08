CC = gcc
CFLAGS = -std=c11 -g -I../lib -lssl -lcrypto -lz 

OBJ = clntService.o createRepository.o ../lib/hash.o ../lib/send_recv.o ../lib/dieWithError.o ../lib/genKey.o
TARGET = clntService


$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) 

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)