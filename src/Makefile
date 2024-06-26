vpath %.c ../src
vpath %.a ../lib


define libs !=
ls ../lib/ | egrep "\.a" | sed -E "s/lib(.*)\.a/\1/"
endef
LFLAGS:=$(addprefix -l, $(libs))

CC=gcc
BIN:=ncm2dump
TARGET:=$(addprefix ../bin/,$(BIN))
#$(info $(TARGET))
CFLAGS=-I../include/ -O3 
LDFLAGS=-L../lib/
LDLIBS=$(LFLAGS) -lpthread 
SRC=$(wildcard *.c)

$(TARGET):
	$(CC) -o $@ $(CFLAGS) $(SRC) $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf $(TARGET)
