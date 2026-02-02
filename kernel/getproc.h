#ifndef GETPROC_H
#define GETPROC_H

enum procstate { 
  UNUSED, 
  USED, 
  SLEEPING, 
  RUNNABLE, 
  RUNNING, 
  ZOMBIE 
};

struct procinfo{
    int pid;
    enum procstate state;
    uint64 sz;
    char name[16];
};

#endif