#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];
int flag_n = 0;
int line = 0;

int
readline(int fd, char* buf, int maxlen){
  int n;
  char c;
  int i = 0;

  while ((n = read(fd, &c, 1)) > 0){
    buf[i] = c;
    if (c == '\n') break;
    i += 1;
    if (i >= (maxlen - 1)){
        fprintf(2, "readline() - line too long\n");
        exit(-1);
    }
  }

  if (((n == 0) && (i == 0)) || (n < 0)){
    return n;
  }

  i += 1;
  buf[i] = '\0';
  return i;
}

void
format_line_index(char *buf, int val)
{
    for(int i = 0; i < 10; i++) {
        buf[i] = ' ';
    }
    buf[10] = '\0';

    if (val == 0) {
        buf[7] = '0';
        return;
    }

    int i = 7; 
    while (val > 0 && i >= 0) {
        buf[i] = (val % 10) + '0'; 
        val /= 10;
        i--;
    }
    
    if (val > 0){
      fprintf(2, "format_line_index() - too many lines\n");
      exit(-1);
    }
}

void
cat(int fd)
{
  int n;
  if (!flag_n){
    while((n = read(fd, buf, sizeof(buf))) > 0) {
      if (write(1, buf, n) != n) {
        fprintf(2, "cat: write error\n");
        exit(1);
      }
    }
    if(n < 0){
      fprintf(2, "cat: read error\n");
      exit(1);
    }
  }
  else{
    char line_buf[11];
    while((n = readline(fd, buf, sizeof(buf))) > 0)
    {
      line++;
      format_line_index(line_buf, line);
      if (write(1, line_buf, 10) != 10){
        fprintf(2, "cat: write error\n");
        exit(1);
      }
      if (write(1, buf, n) != n){
        fprintf(2, "cat: write error\n");
        exit(1);
      }
    }
    if(n < 0){
      fprintf(2, "cat: read error\n");
      exit(1);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  int limit_argc = 1;

  if (argc > 1 && argv[1][0] == '-'){
    char *p = argv[1] + 1;
    while (*p){
      if (*p == 'n'){
        flag_n = 1;
        limit_argc++;
      }
      p++;
    }
  }
  
  if(argc <= limit_argc){
    cat(0);
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if (argv[i][0] == '-') continue;
    if((fd = open(argv[i], O_RDONLY)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
