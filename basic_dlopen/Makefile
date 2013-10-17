PROGRAM=basic_dlopen

$(PROGRAM):
	gcc -c -o $(PROGRAM).o $(PROGRAM).c
	gcc $(PROGRAM).o -ldl -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(PROGRAM).o
