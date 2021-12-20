CFLAGS=-std=c11 -g -static
HELPER_DIRS = ./helper
HELPER_SRCS=$(wildcard *.c)
HELPER_OBJS=$(HELPER_SRCS:.c=.o)

$(HELPER_OBJS):
	$(CC) -c $(HELPER_SRCS) $(LDFLAGS)

clean:
	rm -f *.o
