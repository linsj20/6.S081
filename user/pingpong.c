#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2], n;
    char s[10];
    pipe(p);
    int pid = fork();
    if(pid > 0){
      write(p[1],"p",1);
      close(p[1]);
      wait(0);
      n = read(p[0], s, 1);
      if(n == 0){
        fprintf(2, "parent not received message");
        exit(1);
      }
      fprintf(0, "%d: received pong\n", getpid());
      close(p[0]);
      exit(0);
    }
    else{
      int n;
      char s[10];
      n = read(p[0], s, 1);
      if(n == 0){
        fprintf(2, "child not received message");
        exit(1);
      }
      fprintf(0, "%d: received ping\n", getpid());
      write(p[1], "c", 1);
      close(p[0]);close(p[1]);
      exit(0);
    }
}
