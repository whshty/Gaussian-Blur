all:
	gcc -o serial serial.c -lm
	gcc -o fork fork.c -lm -lrt