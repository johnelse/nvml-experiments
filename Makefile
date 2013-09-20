PROGRAM=get_device_temperatures

$(PROGRAM):
	gcc -lnvidia-ml get_device_temperatures.c -o $@

clean:
	rm -f $(PROGRAM)
