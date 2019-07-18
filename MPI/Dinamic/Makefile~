CC = gcc
CFLAGS = -g -m32

all: bmpFilter

bmpFilter: bmpBlackWhite.o
	mpicc bmpBlackWhite.o -o bmp bmpFilter.c
	mpiexec -hostfile machines -np 10 ./bmp indian.bmp indian2.bmp 150 2

clean:
	rm -f bmpFilter
