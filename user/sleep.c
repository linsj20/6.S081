#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "sleep:you haven't input an argument,dummy:)\n");
    exit(1);
  }

  int t = atoi(argv[1]);
  sleep(t);
  exit(0);
}
