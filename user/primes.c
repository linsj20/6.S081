#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int candis[34];
  int p[2];
  pipe(p);
  for(int i = 2;i < 36; i++){
    write(p[1], &i, 4);
  }
  close(p[1]);
  int pid;
  while(1){
    int n = read(p[0], candis, 136);
    close(p[0]);
    candis[n/4] = 0;
    pipe(p);
    int cur_prime = candis[0];
    fprintf(0, "prime %d\n", cur_prime);
    if(candis[1] == 0){close(p[0]);close(p[1]);break;}
    for(int i = 1; i < 34; i++){
      if(candis[i] == 0){break;}
      if(candis[i] % cur_prime != 0){
        write(p[1], &candis[i], 4);
      }
    }
    close(p[1]);
    pid = fork();
    if(pid > 0){wait(0);exit(0);}
  }
  exit(0);
}
