CC=gcc
TARGET=ncm2dump
INCLUDE=-I./include/
LIBS=-L./lib/ -lbase64 -lcjson -lpthread
SRC=$(wildcard ./src/*.c)
$TARGET:
	$(CC) -o $(TARGET) $(INCLUDE) -O2 $(SRC) $(LIBS)

clean:
	rm -rf $(TARGET)
