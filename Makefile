
FLAGS=-O3

isrunning: isrunning.o
	$(CC) $(FLAGS) isrunning.o isrunning-stand-alone.c -o isrunning

isrunning.o: isrunning.c
	$(CC) $(FLAGS) isrunning.c -c -o isrunning.o

clean:
	rm -f *.o *~ isrunning
