CFLAGS=-Wall -Werror -std=gnu99 -pedantic -ggdb3
OBJS=my_malloc.o
PROGRAM=my_malloc

$(PROGRAM): $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS)

%.o: %.c my_malloc.h
	gcc -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(PROGRAM) *~
