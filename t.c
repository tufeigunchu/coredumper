//g++ t.c -o tt src/coredumper.c src/elfcore.cc src/linuxthreads.cc src/thread_lister.c  -g -no-pie -lzstd
#include "include/coredumper/coredumper.h"
#include <stdio.h>
#include <sys/mman.h>
#include <zstd.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
void fun1(int sig)
{
        int a=1;
    a++;
    a+=10;
    //WriteCoreDump("gg.core");
    int fd = GetCoreDump();
    if(fd>0){
        FILE *fp = fopen("core.zst", "w");
        if(fp){
                char buff[8192];
                ZSTD_CCtx* cctx = ZSTD_createCCtx();
                size_t cLen = ZSTD_compressBound(sizeof(buff));
                void *cBuf = malloc(cLen);
                while(1){
                        int rc = read(fd, buff, sizeof(buff));
                        if (rc < 0) {
                          if (errno == EINTR) continue;
                          break;
                        } else if (rc == 0) {
                          break;
                        }
                        size_t const cSize = ZSTD_compressCCtx(cctx, cBuf, cLen, buff, rc, 1);
                        fwrite(cBuf, 1, cSize, fp);
                }
                ZSTD_freeCCtx(cctx);
                fclose(fp);
                free(cBuf);
        }
    }
    _exit(0);
    //return a;
}

int fun2(void)
{
        return 1;
}

int fun3(int b)
{
        return fun2()+b;
}

int main(int argc, char **argv)
{
        signal(SIGPIPE, fun1);
        char *ptr = (char*)mmap(NULL, 1024*1024*1024, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,-1, 0);
        if(ptr==MAP_FAILED)
        {
                printf("gg!\n");
        }
        else{
                ptr[1]=1;
        }
        //getchar();
        //madvise(ptr, 1024*1024*1024, MADV_DONTDUMP);
        getchar();
        return fun3(argc+2);
}
