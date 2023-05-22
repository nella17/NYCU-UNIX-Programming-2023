#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#define SIZE 10
#define errquit(m) { perror(m); exit(-1); }

const char asmfork[] = "j9X\x0f\x05\x90\x90\xCC";

void dump_regs(struct user_regs_struct regs) {
	fprintf(stderr, "[%s] rax=%-16llx rbx=%-16llx rcx=%-16llx rdx=%-16llx\n",
		"", regs.rax, regs.rbx, regs.rcx, regs.rdx);
	fprintf(stderr, "[%s]  r8=%-16llx  r9=%-16llx r10=%-16llx r11=%-16llx\n",
		"", regs.r8, regs.r9, regs.r10, regs.r11);
	fprintf(stderr, "[%s] r12=%-16llx r13=%-16llx r14=%-16llx r15=%-16llx\n",
		"", regs.r12, regs.r13, regs.r14, regs.r15);
	fprintf(stderr, "[%s] rdi=%-16llx rsi=%-16llx rbp=%-16llx rsp=%-16llx\n",
		"", regs.rdi, regs.rsi, regs.rbp, regs.rsp);
	fprintf(stderr, "[%s] rip=%-16llx\n", "", regs.rip);
	fprintf(stderr, "----\n");
}

#define PTRACE_WAIT(pid, status) \
    ptrace(PTRACE_CONT, pid, 0, 0); \
    if (waitpid(pid, &status, 0) < 0) errquit("waitpid")

int main(int argc, char const *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "usage: %s binary\n", argv[0]);
		return -1;
	}

    const char* bin = argv[1];

	pid_t child;
    if ((child = fork()) < 0) errquit("fork");

    if (child == 0) {
        // if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) errquit("ptrace(TRACEME)");
        execlp(bin, bin, NULL);
        errquit("execlp");
    }

    fprintf(stderr, "[*] child = %d\n", child);

	int status;
	if (ptrace(PTRACE_ATTACH, child, 0, 0) < 0) errquit("ptrace(ATTACH)");
    if (waitpid(child, &status, 0) < 0) errquit("waitpid");
    assert(WIFSTOPPED(status));
    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL | PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE);

    struct user_regs_struct regs;

    for (int i = 0; i < 3; i++) {
        PTRACE_WAIT(child, status);
        assert(WIFSTOPPED(status));
    }

    if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0) errquit("ptrace(GETREGS)");
    char* magic = (char*)regs.rax;
    fprintf(stderr, "[*] magic = %p\n", magic);

    PTRACE_WAIT(child, status);
    assert(WIFSTOPPED(status));

    if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0) errquit("ptrace(GETREGS)");
    void* main = (void*)regs.rip;
    fprintf(stderr, "[*] main  = %p\n", main);

    void* fork = main - strlen(asmfork);
    assert(strlen(asmfork) == sizeof(long));
    long dat = *((long*)asmfork);
    if (ptrace(PTRACE_POKETEXT, child, fork, dat) < 0) errquit("ptrace(POKETEXT)");
    fprintf(stderr, "[*] fork  = %p\n", fork);
    fprintf(stderr, "[*] asm   = 0x%lx\n", dat);

    // dump_regs(regs);

    for (long long n = 0; n < (1<<SIZE); n++) {
        if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0) errquit("ptrace(GETREGS)");
        regs.rip = (long)fork;
        if (ptrace(PTRACE_SETREGS, child, 0, &regs) < 0) errquit("ptrace(POKEUSER)");

        pid_t worker = -1;

        for (int i = 0; i < 2; i++) {
            PTRACE_WAIT(child, status);
            assert(WIFSTOPPED(status));
        }

        if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0) errquit("ptrace(GETREGS)");
        // dump_regs(regs);
        worker = regs.rax;

        if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0) errquit("ptrace(GETREGS)");
        // dump_regs(regs);

        // fprintf(stderr, "[*] worker= %d\n", worker);

        PTRACE_WAIT(worker, status);
        assert(WIFSTOPPED(status));

        // if (ptrace(PTRACE_GETREGS, worker, 0, &regs) < 0) errquit("ptrace(GETREGS)");
        // dump_regs(regs);

        char buf[8];
        for (int i = 0; i < 8; i++)
            buf[i % 8] = '0' + ((n >> i) & 1);
        if (ptrace(PTRACE_POKETEXT, worker, magic, *(long*)buf) < 0) errquit("ptrace(POKETEXT)");
        memset(buf, 0, 8);
        for (int i = 8; i < SIZE; i++)
            buf[i % 8] = '0' + ((n >> i) & 1);
        if (ptrace(PTRACE_POKETEXT, worker, magic+8, *(long*)buf) < 0) errquit("ptrace(POKETEXT)");

        for (;;) {
            PTRACE_WAIT(worker, status);
            if (!WIFSTOPPED(status)) break;
            if (ptrace(PTRACE_GETREGS, worker, 0, &regs) < 0) errquit("ptrace(GETREGS)");
            // dump_regs(regs);
            if (regs.rax == 0xffffffff) break;
        }
        kill(worker, SIGKILL);
        if (!WIFSTOPPED(status)) break;
    }

	return 0;
}
