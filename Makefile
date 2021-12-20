CFLAGS=-std=c11 -g -static
SRC_DIRS = ./src
SRCS=$(wildcard $(SRC_DIRS)/*.c)
OBJS=$(SRCS:.c=.o)
HELPER_DIRS = ./helper
HELPER_OBJS=$(wildcard $(HELPER_DIRS)/*.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): $(SRC_DIRS)/9cc.h

helper:
	make -C $(PWD)/helper

show_helper:
	echo $(HELPER_OBJS)

test: 9cc
	./test.sh

clean:
	rm -f 9cc $(SRC_DIRS)/*.o *~ tmp*
	make clean -C  $(PWD)/helper

.PHONY: test clean
