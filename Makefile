CC=gcc
TARGET=ncm2dump
INCLUDE=-I./include/
LIBS=-L./lib/ -lcjson -lpthread
SRC=$(wildcard ./src/*.c)
$TARGET:
	$(CC) -o $(TARGET) $(INCLUDE) -O2 $(SRC) $(LIBS)

clean:
	rm -rf $(TARGET)
