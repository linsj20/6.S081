#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: xargs <command> <args>");
    exit(1);
  }
  char buf[128];
  char *args[MAXARG];
  for(int i = 0; i < argc - 1; i++){
    args[i] = argv[i + 1];
  }
  int pid;
  int cur = 0;
  args[argc - 1] = buf;
  char* tg = args[argc - 1];
  char temp;
  while(read(0, &temp, 1) > 0){
    switch(temp){
      case ' ':
        tg[cur++] = 0;
        break;
      case '\n':
        tg[cur++] = 0;
        pid = fork();
        if(pid == 0){
          printf("exec...");
          exec(args[0], args);
          fprintf(2, "exec failed");
          exit(1);
        }
        else {wait(0);}
        cur = 0;
        break;
      default:
        tg[cur++] = temp;
        break;
    }
  }
  exit(0);
}
