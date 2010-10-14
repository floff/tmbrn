all: tmbrn

tmbrn: tmbrn.c
	gcc -pedantic -ansi ./tmbrn.c -o ./tmbrn

clean:
	rm -f ./tmbrn

