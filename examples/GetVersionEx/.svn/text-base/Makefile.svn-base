CC=cl
CFLAGS= /nologo /MT /O2 /TC /Zi
LINKS=/link user32.lib /OUT:GetVersionEx.exe /SUBSYSTEM:CONSOLE

all: GetVersionEx 

GetVersionEx: GetVersionEx.c
	$(CC) $(CFLAGS) GetVersionEx.c $(LINKS)

clean:
	del *.exe; del *.obj *.pdb *.ilk
