process: parent.c main.c child.c
	c89 -o process parent.c main.c child.c -I. -Wall -ansi -pedantic-errors
clean:
	rm ./process
