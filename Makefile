PROGRAM=get_device_temperatures

$(PROGRAM):
	gcc -c -o $(PROGRAM).o $(PROGRAM).c
	gcc $(PROGRAM).o -lnvidia-ml -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(PROGRAM).o
