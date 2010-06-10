CC=cl
CFLAGS= /nologo /MT /O2 /TC /Zi
LINKS=/link kernel32.lib /OUT:siphasher.exe /SUBSYSTEM:CONSOLE

all: siphasher 

siphasher: siphasher.c
	$(CC) $(CFLAGS) siphasher.c $(LINKS)

clean:
	del *.exe; del *.obj *.pdb *.ilk
