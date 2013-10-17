PROGRAM=ml_dlopen

$(PROGRAM):
	ocamlopt $(PROGRAM)_stubs.c $(PROGRAM).ml -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) *.cmi *.cmx *.o
