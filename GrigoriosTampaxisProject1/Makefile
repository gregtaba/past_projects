CC = gcc
CFLAGS = -Wall -Wextra -g

all: jobExecutorServer jobCommander setPermissions

jobExecutorServer: function.o jobExecutorServer.o
	$(CC) $(CFLAGS) -o $@ $^

jobCommander: jobCommander.o
	$(CC) $(CFLAGS) -o $@ $^

function.o: function.c function.h
	$(CC) $(CFLAGS) -c $<

jobExecutorServer.o: jobExecutorServer.c function.h
	$(CC) $(CFLAGS) -c $<

jobCommander.o: jobCommander.c
	$(CC) $(CFLAGS) -c $<

setPermissions:
	chmod +x allJobsStop.sh
	chmod +x multijob.sh

clean:
	rm -f *.o jobExecutorServer jobCommander
