#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>

int main(int argc, char **argv)
{
  int probeport = 0;
  struct hostent *host;
  int err, i, net;
  struct sockaddr_in sa;

  if (argc != 2) {
   printf("用法: %s hostname\n", argv[0]);
   exit(1);
  }

  for (i = 1; i < 1024; i++) { //这里有点不是很好，可以将主机地址放在循环外
   strncpy((char *)&sa, "", sizeof sa);
   sa.sin_family = AF_INET;
   if (isdigit(*argv[1]))
    sa.sin_addr.s_addr = inet_addr(argv[1]);
   else if ((host = gethostbyname(argv[1])) != 0)
    strncpy((char *)&sa.sin_addr, (char *)host->h_addr, sizeof sa.sin_addr);
   else {
    herror(argv[1]);
    exit(2);
   }
   sa.sin_port = htons(i);
   net = socket(AF_INET, SOCK_STREAM, 0);
   if (net < 0) {
    perror("\nsocket");
    exit(2);
   }
   err = connect(net, (struct sockaddr *) &sa, sizeof sa);
   if (err < 0) {
    printf("%s %-5d %s\r", argv[1], i, strerror(errno));
    fflush(stdout);
   } else {
    printf("%s %-5d accepted.                     \n", argv[1], i);
    if (shutdown(net, 2) < 0) {
   perror("\nshutdown");
   exit(2);
    }
   }
   close(net);
  }
  printf("                                           \r");
  fflush(stdout);
  return (0);
}
下面这个又是一个端口器：
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "netdb.h"
struct hostent *gethostbyaddr();
void bad_addr();
main(argc, argv)
      int    argc;
      char   *argv[];
{
      char        addr[4];
      int         i, j,
                a0, a1, a2, a3,
                c,
                classB, classC, single, hex;
      char        *fmt = "%d.%d.%d";
      char        **ptr;
      struct hostent  *host;
      extern char    *optarg;
      classB = classC = single = hex = 0;
      while((c = getopt(argc,argv,"bcsx")) != EOF) {
           switch(c) {
           case &#39;b&#39;:
                classB++;
                break;
           case &#39;c&#39;:
                classC++;
                break;
           case &#39;s&#39;:
                single++;
                break;
           case &#39;x&#39;:
                hex++;
                break;
           }
      }
      if(classB == 0 && classC == 0 && single == 0) {
           fprintf(stderr, "usage: %s [-b||-c||-s] [-x] xxx.xxx[.xxx[.xxx]]\n", argv[0]);
           exit(1);
      }
      if(classB)
           if(hex) {
                fmt = "%x.%x";
                sscanf(argv[3], fmt, &a0, &a1);
           } else {
                fmt = "%d.%d";
                sscanf(argv[2], fmt, &a0, &a1);
           }
      else if(classC)
           if(hex) {
                fmt = "%x.%x.%x";
                sscanf(argv[3], fmt, &a0, &a1, &a2);
           } else {
                fmt = "%d.%d.%d";
                sscanf(argv[2], fmt, &a0, &a1, &a2);
           }
      else if(single)
           if(hex) {
                fmt = "%x.%x.%x.%x";
                sscanf(argv[3], fmt, &a0, &a1, &a2, &a3);
           } else {
                fmt = "%d.%d.%d.%d";
                sscanf(argv[2], fmt, &a0, &a1, &a2, &a3);
           }
      sscanf(argv[1], fmt, &a0, &a1, &a2);
      addr[0] = (unsigned char)a0;
      addr[1] = (unsigned char)a1;
      if(a0>255||a0<0)
           bad_addr(a0);
      if(a1>255||a1<0)
           bad_addr(a1);
      if(classB) {
           if(hex)
                printf("Converting address from hex. (%x.%x)\n", a0, a1);
           printf("Scanning Class B network %d.%d...\n", a0, a1);
           while(j!=256) {
                a2=j;
                addr[2] = (unsigned char)a2;
jmpC:
           if(classC)
                if(hex)
                      printf("Converting address from hex. (%x.%x.%x)\n", a0, a1, a2);
                printf("Scanning Class C network %d.%d.%d...\n", a0, a1, a2);
           while(i!=256) {
                a3=i;
                addr[3] = (unsigned char)a3;
jmpS:
                if ((host = gethostbyaddr(addr, 4, AF_INET)) != NULL) {
                      printf("%d.%d.%d.%d => %s\n", a0, a1, a2, a3, host->h_name);
                      ptr = host->h_aliases;
                      while (*ptr != NULL) {
                           printf("%d.%d.%d.%d => %s (alias)\n", a0, a1, a2, a3, *ptr);
                           ptr++;
                      }
                }
                if(single)
                      exit(0);
                i++;
           }
           if(classC)
                exit(0);
           j++;
           }
      } else if(classC) {
           addr[2] = (unsigned char)a2;
           if(a2>255||a2<0)
                bad_addr(a2);
           goto jmpC;
      } else if(single) {
           addr[2] = (unsigned char)a2;
           addr[3] = (unsigned char)a3;
           if(a2>255||a2<0)
                bad_addr(a2);
           if(a3>255||a3<0)
                bad_addr(a3);
           goto jmpS;
      }
      exit(0);
}
void
bad_addr(addr)
      int *addr;
{
      printf("Value %d is not valid.\n", addr);
      exit(0);
}
