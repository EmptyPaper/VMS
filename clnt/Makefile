CC = gcc
CFLAGS = -std=c11 -g -I../lib -lssl -lcrypto -lz

OBJ = servService.o createRepository.o lib/handleClnt.o ../lib/hash.o ../lib/send_recv.o ../lib/dieWithError.o
TARGET = servService

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) 

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)