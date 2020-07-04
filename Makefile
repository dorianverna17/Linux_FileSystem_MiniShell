build: file.h fsminishell.c
	gcc -o fsminishell file.h fsminishell.c
clean: fsminishell
	rm fsminishell
run: fsminishell
	./fsminishell
