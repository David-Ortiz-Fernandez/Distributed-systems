CC = gcc
CFLAGS = -g -m32

all: bmpFilter

bmpFilter: bmpBlackWhite.o
	mpicc bmpBlackWhite.o -o bmp bmpFilter.c
	mpiexec -hostfile machines -np 5 ./bmp indian.bmp indian2.bmp 230

clean:
	rm -f bmpFilter
