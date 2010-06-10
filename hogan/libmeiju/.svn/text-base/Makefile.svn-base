CC=i386-mingw32-gcc
#CFLAGS= /nologo /MT /O2 /TC
CFLAGS=-c
#LINKS=/link kernel32.lib /OUT:dl.exe /SUBSYSTEM:CONSOLE
LINKS=-o libmeiju.o

all: meiju 

meiju: meiju.c
	$(CC) $(CFLAGS) meiju.c $(LINKS)
	ar rcs libmeiju.a 

clean:
	rm *.exe; rm *.obj; rm *.o;
