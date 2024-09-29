CC = gcc
CFLAGS = -Wall -g
OBJS = main.o execute.o func.o parsing.o  

TARGET = SISH

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c shell.h
	$(CC) $(CFLAGS) -c main.c

parsing.o: parsing.c shell.h
	$(CC) $(CFLAGS) -c parsing.c

execute.o: execute.c shell.h
	$(CC) $(CFLAGS) -c execute.c

func.o: func.c shell.h  
	$(CC) $(CFLAGS) -c func.c

clean:
	rm -f *.o $(TARGET)
