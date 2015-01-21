OBJ=ringbuffer.o common_test.o

all: clean $(OBJ)
	g++ -g -Wall -O0 -lpthread -o common_test $(OBJ)

ringbuffer.o: ringbuffer.c ringbuffer.h
common_test.o: common_test.cpp


clean:
	-rm *.o common_test
