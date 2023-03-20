#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <dlfcn.h>

#include "./lab03_pub/shuffle.h"
#include "gots.h"

// modify from ./lab03_pub/libpoem.c
#define errquit(m)	{ perror(m); _exit(-1); }
static long chal_base = 0;
static long poem_base = 0;
static void get_base() {
	int fd, sz;
	char buf[16384], *s = buf, *line, *saveptr;
	if((fd = open("/proc/self/maps", O_RDONLY)) < 0) errquit("get_base/open");
	if((sz = read(fd, buf, sizeof(buf)-1)) < 0) errquit("get_base/read");
	buf[sz] = 0;
	close(fd);
    // puts(buf);
	while((line = strtok_r(s, "\n\r", &saveptr)) != NULL) { s = NULL;
        long min, max;
        if(sscanf(line, "%lx-%lx ", &min, &max) != 2) errquit("get_base/sscanf");
		if(poem_base == 0 && strstr(line, "/libpoem.so") != NULL)
            poem_base = min;
		if(chal_base == 0 && strstr(line, "/chal") != NULL)
            chal_base = min;
	}
}

inline void fail(const char* s) {
    if (errno) perror(s);
    else fprintf(stderr, "%s: unknown error", s);
#ifndef DEBUG
    exit(-1);
#endif
}

int init() {
    setvbuf(stdout, NULL, _IONBF, 0);

    get_base();
    size_t size = sizeof(ndat) / sizeof(int);
    printf("size = %ld\n", size);
    printf("pid = %d\nchal base = 0x%lx\npoem base = 0x%lx\n", getpid(), chal_base, poem_base);

    if (mprotect((void*)(chal_base + 0x17000), 0x2000, PROT_READ | PROT_WRITE) < 0)
        fail("mprotect");

    void *handle = dlopen("libpoem.so", RTLD_LAZY);

    int* rndat = malloc(sizeof(int) * size);
    void** fps = malloc(sizeof(void*) * size);
    char buf[10];
    for (int i = 0; i < size; i++) {
        sprintf(buf, "code_%d", i);
        rndat[ ndat[i] ] = i;
        fps[i] = dlsym(handle, buf);
    }


    printf("0x%lx %p\n", chal_base + gots[1], fps[1]);
    /* getchar(); */

    for (int i = 0; i < size; i++)
        if (gots[i]) {
            if (*(void**)(chal_base+gots[i]) != fps[i])
                printf("%d\n", i);
            /*
            printf("%d: %p %p -> %d %lx\n", i,
                (void**)(chal_base+gots[i]),
                *(void**)(chal_base+gots[i]),
                rndat[i],
                poem_base + fps[rndat[i]]
            );
            */
            *(void**)(chal_base+gots[i]) = fps[rndat[i]];
        }

    free(rndat);
    free(fps);

    return 0;
}
