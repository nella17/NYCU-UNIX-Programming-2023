#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <elf.h>
#include <capstone/capstone.h>

#include <map>
#include <set>

constexpr int MAX_INSTR_BYTES = 15 + 1;
using ull = unsigned long long;

int wait_stop(int pid) {
    int status;
    if (waitpid(pid, &status, 0) < 0)
        throw ((perror("waitpid"), -1));
    if (WIFEXITED(status)) {
        puts("** the target program terminated.");
        throw true;
    } else {
        assert(WIFSTOPPED(status));
    }
    return status;
}

auto get_regs(int pid) {
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, 0, &regs) < 0)
        throw ((perror("ptrace(GETREGS)"), -1));
    return regs;
}

void set_regs(pid_t pid, struct user_regs_struct regs) {
    if (ptrace(PTRACE_SETREGS, pid, 0, &regs) < 0)
        throw ((perror("ptrace(SETREGS)"), -1));
}

long readlong(pid_t pid, ull addr) {
    errno = 0;
    long data = ptrace(PTRACE_PEEKTEXT, pid, addr);
    if (errno != 0)
        throw ((perror("ptrace(PEEKTEXT)"), -1));
    return data;
}

uint8_t readbyte(pid_t pid, ull addr) {
    long byte = readlong(pid, addr);
    return byte & 0xff;
}

void writelong(pid_t pid, ull addr, long data) {
    if (ptrace(PTRACE_POKETEXT, pid, addr, data) < 0)
        throw ((perror("ptrace(POKETEXT)"), -1));
}

void writebyte(pid_t pid, ull addr, uint8_t byte) {
    long data = readlong(pid, addr);
    data = (data & ~0xff) | byte;
    writelong(pid, addr, data);
}

int kill(pid_t pid) {
    kill(pid, SIGKILL);
    int status;
    waitpid(pid, &status, 0);
    return status;
}

class SDB {
public:
    static constexpr int DISASSEMBLE_COUNT = 5;
    static constexpr uint8_t NOP = 0x90;
    static constexpr uint8_t INT = 0xCC;
    static constexpr long ASMFORK = (long)0xcc9090050f58396a;

    const char* binpath;
    size_t binsize;
    const char* binary;
    csh handle;
    pid_t child, child_anchor;
    struct user_regs_struct regs;
    uint8_t code[MAX_INSTR_BYTES * 5];
    ull text_start, text_end;
    std::map<ull, uint8_t> breakpoints;
    std::set<pid_t> childs;

    int wait_stop_regs() {
        int status = wait_stop(child);
        regs = get_regs(child);
        if (breakpoints.count(regs.rip-1))
            regs.rip--, set_regs(child, regs);
        if (breakpoints.count(regs.rip))
            printf("** hit a breakpoint at 0x%llx.\n", regs.rip);
        return status;
    }

    void show_disassemble() {
        for (size_t i = 0; i < sizeof(code); i += sizeof(long))
            *(long*)&code[i] = readlong(child, regs.rip + i);

        for (auto [addr, byte]: breakpoints) {
            auto off = addr - regs.rip;
            if (0 <= off and off < sizeof(code))
                code[off] = byte;
        }

        cs_insn *insns;
        size_t count = cs_disasm(handle, code, sizeof(code), regs.rip, 0, &insns);
        if (count <= 0) {
            printf("** Failed to disassemble given code at 0x%llx!\n", regs.rip);
            return;
        }

        for (size_t j = 0; j < DISASSEMBLE_COUNT and j < count; j++) {
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
    }

    char* mapbin() {
        int fd = open(binpath, O_RDONLY);
        if (fd < 0) throw ((perror("open"), -1));
        struct stat buf;
        fstat(fd, &buf);
        binsize = (size_t)buf.st_size;
        auto mp = (char*)mmap(NULL, binsize, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        return mp;
    }

    SDB(char* const argv[]):
        binpath(argv[0]),
        binsize{},
        binary(mapbin()),
        handle{},
        child(-1), child_anchor(-1),
        regs{},
        code{},
        text_start(0), text_end((ull)-1ll),
        breakpoints{},
        childs{}
    {
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
            throw ((puts("cs_open(fail"), -1));

        auto ehdr = (Elf64_Ehdr*)binary;
        if (strncmp((const char*)ehdr->e_ident, "\x7F\x45\x4C\x46", 4))
            throw ((puts("not elf ?"), -1));

        auto shdrs = (Elf64_Shdr*)&binary[ehdr->e_shoff];

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
        if (child < 0) throw ((perror("fork"), -1));

        if (child == 0) {
            if (ptrace(PTRACE_TRACEME) < 0)
                throw ((perror("ptrace(TRACEME)"), -1));
            execv(binpath, argv);
            throw ((perror("execvp"),-1));
        }

        childs.emplace(child);

        wait_stop_regs();
        if (ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL | PTRACE_O_TRACEFORK) < 0)
            throw ((perror("ptrace(SETOPTIONS)"), -1));
        if (regs.rip != ehdr->e_entry)
            throw ((puts("not static binary ?"), -1));
        printf("** program '%s' loaded. entry point 0x%lx\n", binpath, ehdr->e_entry);
    }

    ~SDB() {
        cs_close(&handle);
        munmap((void*)binary, binsize);
        for (auto pid: childs)
            kill(pid);
    }

    void step() {
        auto prip = regs.rip;
        bool hasbp = breakpoints.count(prip);
        if (hasbp) writebyte(child, prip, breakpoints[prip]);
        if (ptrace(PTRACE_SINGLESTEP, child, 0, 0) < 0)
            throw (perror("ptrace(SINGLESTEP)"), -1);
        wait_stop_regs();
        if (hasbp) writebyte(child, prip, INT);
    }

    void cont() {
        if (breakpoints.count(regs.rip))
            step();
        if (breakpoints.count(regs.rip))
            return;
        if (ptrace(PTRACE_CONT, child, 0, 0) < 0)
            throw (perror("ptrace(CONT)"), -1);
        wait_stop_regs();
    }

    void breakpoint(ull addr) {
        if (breakpoints.count(addr)) return;
        uint8_t byte = readbyte(child, addr);
        breakpoints.emplace(addr, byte);
        writebyte(child, addr, INT);
    }

    void anchor() {
        if (child_anchor >= 0) kill(child_anchor);

        long data = readlong(child, regs.rip);

        writelong(child, regs.rip, ASMFORK);
        for (int i = 0; i < 2; i++) {
            if (ptrace(PTRACE_CONT, child, 0, 0) < 0)
                throw (perror("ptrace(CONT)"), -1);
            wait_stop(child);
        }
        child_anchor = (pid_t)get_regs(child).rax;
        if (child_anchor < 0)
            throw (puts("** can't create anchor"), -1);
        childs.emplace(child_anchor);

        writelong(child_anchor, regs.rip, data);
        set_regs(child_anchor, regs);
        for (auto [addr, byte]: breakpoints)
            writebyte(child_anchor, addr, byte);

        writelong(child, regs.rip, data);
        set_regs(child, regs);
    }

    void timetravel() {
        if (child_anchor < 0) {
            puts("** anchor point not exist");
            return;
        }

        child = child_anchor;
        child_anchor = -1;
        regs = get_regs(child);
        anchor();

        for (auto [addr, byte]: breakpoints)
            writebyte(child, addr, INT);
    }

    void run() {
        char cmd[0x100];
        bool show = true;
        while (true) {
            if (show) {
                regs = get_regs(child);
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
                case 'b': { // break
                    ull addr;
                    sscanf(strchr(cmd, ' ')+1, "%llx", &addr);
                    printf("** set a breakpoint at 0x%llx.\n", addr);
                    breakpoint(addr);
                    break;
                }
                case 'a': // anchor
                    puts("** dropped an anchor");
                    anchor();
                    break;
                case 't': // timetravel
                    puts("** go back to the anchor point");
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

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    try {
        SDB sdb(argv+1);
        sdb.run();
    } catch (...) {
    }

    return 0;
}

