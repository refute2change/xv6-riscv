#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

int
main(int argc, char *argv[])
{
  uint64 addr1, addr2, addr3;

  printf("--- Testing Multiple Dynamic Mmaps ---\n");

  // 1. Map 3 distinct regions
  // Region 1: 1 Page
  addr1 = mmap(0, PGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
  // Region 2: 1 Page
  addr2 = mmap(0, PGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
  // Region 3: 2 Pages (8192 bytes)
  addr3 = mmap(0, PGSIZE * 2, PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0); 

  if(addr1 == (uint64)-1 || addr2 == (uint64)-1 || addr3 == (uint64)-1){
    printf("mmap failed\n");
    exit(1);
  }

  // Verify dynamic addressing
  printf("Mapped Region 1 at: %p (Size: 1 Page)\n", (void*)addr1);
  printf("Mapped Region 2 at: %p (Size: 1 Page)\n", (void*)addr2);
  printf("Mapped Region 3 at: %p (Size: 2 Pages)\n", (void*)addr3);

  // 2. Write to them (This triggers 4 separate Page Faults!)
  *(int*)addr1 = 111;
  *(int*)addr2 = 222;
  
  // Write to both the first AND second page of Region 3
  *(int*)addr3 = 333;
  *((int*)(addr3 + PGSIZE)) = 444; 

  printf("Parent wrote data: %d, %d, %d, %d\n", 
          *(int*)addr1, *(int*)addr2, *(int*)addr3, *((int*)(addr3 + PGSIZE)));

  // 3. Fork and share
  int pid = fork();
  if(pid < 0){
    printf("Fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child process reads the shared data from all 3 regions
    printf("Child read data:   %d, %d, %d, %d\n", 
            *(int*)addr1, *(int*)addr2, *(int*)addr3, *((int*)(addr3 + PGSIZE)));

    // Modify data in Region 2 and the second page of Region 3
    *(int*)addr2 = 999;
    *((int*)(addr3 + PGSIZE)) = 888;
    printf("Child modified Region 2 and Region 3 (Page 2)\n");

    // Unmap JUST Region 1 to test independent munmap
    if(munmap(addr1, PGSIZE) < 0)
      printf("Child: munmap Region 1 failed\n");
    else
      printf("Child: unmapped Region 1 only\n");

    exit(0);
  } else {
    // Parent process waits for child
    wait(0);

    // Verify the child's modifications are visible in the specific regions
    printf("Parent read later: %d, %d, %d, %d\n", 
            *(int*)addr1, *(int*)addr2, *(int*)addr3, *((int*)(addr3 + PGSIZE)));

    // 4. Cleanup
    munmap(addr1, PGSIZE);
    munmap(addr2, PGSIZE);
    munmap(addr3, PGSIZE * 2);
    printf("Parent unmapped all regions\n");
  }

  exit(0);
}