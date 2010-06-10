CC=cl
CFLAGS= /nologo /MT /O2
LINKS=/link kernel32.lib Netapi32.lib /OUT:netjobenum.exe /SUBSYSTEM:CONSOLE

all: netjobenum 

netjobenum: 
	$(CC) $(CFLAGS) netjobenum.cpp $(LINKS)

clean:
	del *.exe; del *.obj
