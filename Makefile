
make:
	gcc main.c libsais64.c libsais.c -o main.o -lm

clean:
	rm -rf *.o