mem: mem.c mem.h
	gcc -g -c -Wall -m32 -std=gnu99 -fpic mem.c -O
	gcc -shared -Wall -m32 -std=gnu99 -o libmem.so mem.o -O

clean:
	rm -rf mem.o libmem.so
