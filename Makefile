CC = gcc
CFLAGS = -Wall -g
OBJS = main.o execute.o utility.o
TARGET = shell_program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c shell.h
	$(CC) $(CFLAGS) -c main.c

execute.o: execute.c shell.h
	$(CC) $(CFLAGS) -c execute.c

utility.o: utility.c shell.h
	$(CC) $(CFLAGS) -c utility.c

clean:
	rm -f *.o $(TARGET)
