CC=cl
CFLAGS= /nologo /MT /O2 /TC
LINKS=/link /OUT:keypull.exe /SUBSYSTEM:CONSOLE

all: keypull 

keypull: keypull.c
	$(CC) $(CFLAGS) keypull.c $(LINKS)

clean:
	del *.exe; del *.obj
