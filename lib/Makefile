src=$(wildcard *.c)
objs=$(subst .c,.o,$(src))
cflags=-I../include
target=libbase64.a

$(target): $(objs)
	$(AR) rcs $@ $^

$(objs): $(src)
	$(CC) -c $(cflags) $^

clean:
	rm -rf $(objs) $(target)

.PHONY: clean
