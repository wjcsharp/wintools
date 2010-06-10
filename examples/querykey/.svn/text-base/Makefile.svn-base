#CC=cl
#CFLAGS= /nologo /MT /O2 /TC
#LINKS=/link user32.lib kernel32.lib Ws2_32.lib /OUT:chijiuxing.exe /SUBSYSTEM:CONSOLE

CC=/opt/local/bin/i386-mingw32-gcc 
CFLAGS= -o querykey.exe
LINKS= -mconsole 

all: querykey 

querykey: querykey.c
	$(CC) $(CFLAGS) querykey.c $(LINKS)

clean:
	rm *.exe; rm *.obj
