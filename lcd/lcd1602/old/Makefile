#CC = gcc 
CC = arm-linux-gnueabihf-gcc 

TARGET	= $(patsubst %.c,%, $(wildcard *.c))
SRC 	= $(wildcard *.c)
SRC 	+= $(wildcard *.h)

$(TARGET):$(SRC)
	$(CC) -o $@ -L ../lib/ -I ../lib/includes/ $^ -lfahw -lm -Wall

.PHONY: clean 
clean:
	rm -f $(TARGET)
