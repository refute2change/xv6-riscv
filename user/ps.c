#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/getproc.h" 

int
main(int argc, char *argv[])
{
  struct procinfo p[64];
  
  if(getprocs(p) < 0){
    fprintf(2, "getprocs failed\n");
    exit(1);
  }

  char *states[] = {
    [UNUSED]   "unused",
    [USED]     "used",
    [SLEEPING] "sleep ",
    [RUNNABLE] "runble",
    [RUNNING]  "run   ",
    [ZOMBIE]   "zombie"
  };

  printf("PID\tSTATE\tSIZE\tNAME\n");

  for(int i = 0; i < 64; i++){
    if(p[i].state != UNUSED){
      printf("%d\t%s\t%ld\t%s\n", 
             p[i].pid, 
             states[p[i].state], 
             p[i].sz, 
             p[i].name);
    }
  }

  exit(0);
}