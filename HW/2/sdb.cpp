#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <elf.h>
#include <capstone/capstone.h>

#include <map>

constexpr int MAX_INSTR_BYTES = 15 + 1;
using ull = unsigned long long;

class SDB {
public:
    static constexpr int DISASSEMBLE_COUNT = 5;

    const char* binpath;
    const char* binary;
    size_t binsize;
    csh handle;
    pid_t child;
    struct user_regs_struct regs;
    uint8_t code[MAX_INSTR_BYTES * 5];
    ull text_start, text_end;
    std::map<ull, uint8_t> breakpoints;

    int wait_stop(int pid) {
        int status;
        if (waitpid(pid, &status, 0) < 0)
            exit((perror("waitpid"), -1));
        if (WIFEXITED(status)) {
            puts("** the target program terminated.");
            throw true;
        } else {
            assert(WIFSTOPPED(status));
        }
        return status;
    }

    void get_regs() {
        if (ptrace(PTRACE_GETREGS, child, 0, &regs) < 0)
            exit((perror("ptrace(GETREGS)"), -1));
    }

    int wait_stop_regs() {
        int status = wait_stop(child);
        get_regs();
        return status;
    }

    long readtext(ull addr) {
        return ptrace(PTRACE_PEEKTEXT, child, addr);
    }

    void show_disassemble() {
        for (size_t i = 0; i < sizeof(code); i += sizeof(long))
            *(long*)&code[i] = readtext(regs.rip + i);

        cs_insn *insns;
        size_t count = cs_disasm(handle, code, sizeof(code), regs.rip, 0, &insns);
        if (count > 0) {
            for (size_t j = 0; j < DISASSEMBLE_COUNT; j++) {
                auto& insn = insns[j];
                if (insn.address >= text_end) {
                    puts("** the address is out of the range of the text section.");
                    break;
                }
                printf("\t0x%lx:", insn.address);
                for (size_t k = 0; k < MAX_INSTR_BYTES; k++)
                    if (k < insn.size)
                        printf(" %02x", insn.bytes[k]);
                    else
                        printf("   ");
                printf("\t%s\t%s\n", insn.mnemonic, insn.op_str);
            }
            cs_free(insns, count);
        } else {
            printf("** Failed to disassemble given code!\n");
            throw false;
        }
    }

    char* mapbin() {
        int fd = open(binpath, O_RDONLY);
        if (fd < 0) exit((perror("open"), -1));
        struct stat buf;
        fstat(fd, &buf);
        binsize = (size_t)buf.st_size;
        auto mp = (char*)mmap(NULL, binsize, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        return mp;
    }

    SDB(char* const argv[]):
        binpath(argv[0]),
        binary(mapbin()),
        breakpoints{}
    {
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
            exit((puts("cs_open(fail"), -1));

        auto ehdr = (Elf64_Ehdr*)binary;
        if (strncmp((const char*)ehdr->e_ident, "\x7f""ELF", 4))
            exit((puts("not elf ?"), -1));

        auto shdrs = (Elf64_Shdr*)(binary + ehdr->e_shoff);

        auto shstrtab = &shdrs[ehdr->e_shstrndx];
        auto shstrn = &binary[shstrtab->sh_offset];

        for (size_t i = 0; i < ehdr->e_shnum; i++) {
            auto shdr = &shdrs[i];
            if (shdr->sh_size and !strcmp(shstrn+shdr->sh_name, ".text")) {
                text_start = shdr->sh_addr;
                text_end = shdr->sh_addr + shdr->sh_size;
            }
        }

        child = fork();
        if (child < 0) exit((perror("fork"), -1));

        if (child == 0) {
            ptrace(PTRACE_TRACEME);
            execv(binpath, argv);
            exit((perror("execvp"),-1));
        }

        wait_stop_regs();
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL | PTRACE_O_TRACEFORK);
        printf("** program '%s' loaded. entry point 0x%llx\n", binpath, regs.rip);
    }

    ~SDB() {
        cs_close(&handle);
        munmap((void*)binary, binsize);
    }

    void step() {
        ptrace(PTRACE_SINGLESTEP, child, 0, 0);
        wait_stop(child);
    }

    void cont() {
        ptrace(PTRACE_CONT, child, 0, 0);
        wait_stop(child);
    }

    void breakpoint(ull addr) {
        // TODO
    }

    void anchor() {
        // TODO
    }

    void timetravel() {
        // TODO
    }

    void run() {
        char cmd[0x100];
        bool show = true;
        while (true) {
            if (show) {
                get_regs();
                show_disassemble();
                show = false;
            }
            printf("(sdb) ");
            fgets(cmd, sizeof(cmd), stdin);
            switch (cmd[0]) {
                case 's': // si
                    step();
                    show = true;
                    break;
                case 'c': // cont
                    cont();
                    show = true;
                    break;
                case 'b': // break
                    breakpoint(strtoull(strchr(cmd, ' ')+1, NULL, 10));
                    break;
                case 'a': // anchor
                    anchor();
                    break;
                case 't': // timetravel
                    timetravel();
                    show = true;
                    break;
            }
        }
    }

};

int main(int argc, char* const argv[]) {
    if (argc < 2) {
        printf("usage: %s [program]\n", argv[0]);
        exit(-1);
    }

    SDB sdb(argv+1);
    try {
        sdb.run();
    } catch (...) {
    }

    return 0;
}

