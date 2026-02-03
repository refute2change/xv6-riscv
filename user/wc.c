#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

// Flags
int flag_l = 0;
int flag_w = 0;
int flag_c = 0;

// Global totals
int total_l = 0;
int total_w = 0;
int total_c = 0;

// Helper to check for whitespace (replaces strchr)
int
isspace(char c)
{
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v';
}

void
wc(int fd, char *name)
{
  int i, n;
  int l, w, c, inword;

  l = w = c = 0;
  inword = 0;
  
  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i<n; i++){
      // printf("%c", buf[i]); // Echo input to stdout
      c++;
      if(buf[i] == '\n')
        l++;
      if(isspace(buf[i]))
        inword = 0;
      else if(!inword){
        w++;
        inword = 1;
      }
    }
  }
  
  if(n < 0){
    printf("wc: read error\n");
    // Don't exit here, just return so we can process other files
    return;
  }

  // Add to global totals
  total_l += l;
  total_w += w;
  total_c += c;

  // Print individual file stats
  if(flag_l) printf("%d ", l);
  if(flag_w) printf("%d ", w);
  if(flag_c) printf("%d ", c);
  printf("%s\n", name);
}

int
main(int argc, char *argv[])
{
  int fd, i;
  int files_processed = 0;
  int file_mode = 0;

  // 1. Detect Flags
  for(i = 1; i < argc; i++){
    if(argv[i][0] == '-'){
      char *p = argv[i] + 1;
      while(*p){
        if(*p == 'l') flag_l = 1;
        else if(*p == 'w') flag_w = 1;
        else if(*p == 'c') flag_c = 1;
        else
        {
          printf("wc: unknown flag %c\n", *p);
          exit(0);
        }
        p++;
      }
    }
  }

  // Default to all if no flags set
  if(!flag_l && !flag_w && !flag_c){
    flag_l = 1; 
    flag_w = 1; 
    flag_c = 1;
  }

  // 2. Process Files
  for(i = 1; i < argc; i++){
    // Skip flag arguments
    if(argv[i][0] == '-') continue;

    printf("Processing file: %s\n", argv[i]);
    file_mode = 1;

    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("wc: cannot open %s\n", argv[i]);
      // CONTINUE instead of EXIT to handle subsequent files
      continue;
    }
    
    wc(fd, argv[i]);
    close(fd);
    files_processed++;
  }
  if (file_mode) {
    exit(0);
  }

  // 3. Handle Stdin (if no files given)
  if(files_processed == 0){
    wc(0, "");
  }
  // 4. Handle Totals (only if multiple files succeeded)
  else if(files_processed > 1){
    if(flag_l) printf("%d ", total_l);
    if(flag_w) printf("%d ", total_w);
    if(flag_c) printf("%d ", total_c);
    printf("total\n");
  }

  exit(0);
}