OBJ:=$(patsubst %.c, %.o, $(wildcard *.c))
stego:$(OBJ)
	gcc -o $@ $^
clean:
	rm *.exe *.o