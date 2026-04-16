#define SBRK_ERROR ((char *)-1)
#define PROT_READ   0x1
#define PROT_WRITE  0x2
#define MAP_SHARED  0x1
#define MAP_PRIVATE 0x2

struct stat;
struct procinfo;


// system calls
int fork(void);
int exit(int) __attribute__((noreturn));
int wait(int*);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(const char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sys_sbrk(int,int);
int pause(int);
int uptime(void);
int getprocs(struct procinfo*);
int trace(int);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
char* gets(char*, int max);
uint strlen(const char*);
void* memset(void*, int, uint);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);
char* sbrk(int);
char* sbrklazy(int);

// printf.c
void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));
void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));

// umalloc.c
void* malloc(uint);
void free(void*);

/* initial mmap() and munmap()
uint64 mmap(void);
int munmap(uint64);
*/

uint64 mmap(uint64 addr, int length, int prot, int flags, int fd, int offset);
int munmap(uint64 addr, int length);

typedef struct {
    uint64 addr;
    int length;
    int prot;
    int flags;
    int fd;
    int offset;
} mmap_args;
typedef struct {
    uint64 addr;
    int length;
} munmap_args;

#define mmap(...) var_mmap((mmap_args){__VA_ARGS__})
#define munmap(...) var_munmap((munmap_args){__VA_ARGS__})

static inline uint64 var_mmap(mmap_args in){
    uint64 addr_out = in.addr ? in.addr: 0;
    int length_out = in.length ? in.length: 4096;
    int prot_out = in.prot ? in.prot: 0x1 | 0x2;
    int flags_out = in.flags? in.flags: 0x1;
    int fd_out = in.fd? in.fd: -1;
    int offset_out = in.offset? in.offset: 0;
    return (mmap)(addr_out, length_out, prot_out, flags_out, fd_out, offset_out);
}
static inline int var_munmap(munmap_args in){
    uint64 addr_out = in.addr ? in.addr: 0;
    int length_out = in.length ? in.length: 4096;
    return (munmap)(addr_out, length_out);
}
