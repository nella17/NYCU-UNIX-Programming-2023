#include "sandbox.hpp"

#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <elf.h>
#include <fcntl.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#define DO_HOOKS(MACRO) \
    MACRO(open) \
    MACRO(close) \
    MACRO(read) \
    MACRO(write) \
    MACRO(connect) \
    MACRO(getaddrinfo) \
    MACRO(system) \

static std::unordered_map<std::string, std::vector<std::string>> configs;
static FILE* logger;
#define log(...) fprintf(logger, __VA_ARGS__);

static std::unordered_map<std::string, void*> hook_funcs;

enum TYPE {
    NONE,
    BEGIN,
    END,
};
inline std::pair<TYPE, std::string> parseline(std::string line) {
    const std::string blacklist = "-blacklist";
    const std::string begin = "BEGIN";
    const std::string end = "END";
    TYPE type = TYPE::NONE;
    size_t pos = 0;
    std::string name = "";
    if (line.rfind(blacklist) + blacklist.size() == line.size()) {
        if (line.find(begin) == 0) {
            type = TYPE::BEGIN;
            pos = begin.size() + 1;
        }
        if (line.find(end) == 0) {
            type = TYPE::END;
            pos = end.size() + 1;
        }
    }
    if (type != TYPE::NONE)
        name = line.substr(pos, line.size() - blacklist.size() - pos);
    return { type, name };
}

static void load_config(const char* config) {
    configs.clear();
    std::ifstream fin(config);
    std::string line, current = "";
    while (std::getline(fin, line)) {
        auto [type, name] = parseline(line);
        if (current.empty()) {
            if (type == TYPE::BEGIN)
                current = name;
        } else {
            if (type == TYPE::END) {
                current.clear();
            } else {
                configs[current].emplace_back(line);
            }
        }
    }
}

static int hook_open(const char* path, int oflag, mode_t mode) {
    for (auto black: configs["open"])
        if (path == black) {
            errno = EACCES;
            return -1;
        }
    return open(path, oflag, mode);
}

static int hook_close(...) { return -1; }
static int hook_read(...) { return -1; }
static int hook_write(...) { return -1; }
static int hook_connect(...) { return -1; }
static int hook_getaddrinfo(...) { return -1; }
static int hook_system(...) { return -1; }

static void patch_got() {
    void* main_handle = dlopen(NULL, RTLD_LAZY);
    const struct link_map * link_map = 0;
    const int ret = dlinfo(main_handle, RTLD_DI_LINKMAP, &link_map);
    assert(ret == 0 and link_map != 0);
    const struct link_map * const loaded_link_map = link_map;
    const ElfW(Dyn) * const dyn_start = loaded_link_map->l_ld;
    const ElfW(Addr) load_addr = loaded_link_map->l_addr;

    const char* strtab;
    const ElfW(Rela)* plt_rela;
    size_t plt_relsz = 0;
    const ElfW(Sym) * dynsym;

    for (const ElfW(Dyn) * dyn = dyn_start; dyn->d_tag != DT_NULL; ++dyn) {
        switch (dyn->d_tag) {
            case DT_STRTAB:
                strtab = (char*)dyn->d_un.d_ptr;
                break;
            case DT_JMPREL:
                plt_rela = (ElfW(Rela)*)dyn->d_un.d_ptr;
                break;
            case DT_PLTRELSZ:
                plt_relsz = dyn->d_un.d_val / sizeof(ElfW(Rela));
                break;
            case DT_SYMTAB:
                dynsym = (ElfW(Sym)*)dyn->d_un.d_ptr;
                break;
        }
    }

#ifdef DEBUG
    fprintf(stderr, "strtab = %p\nplt_rela = %p\nplt_relsz = 0x%lx\ndynsym = %p\n", strtab, plt_rela, plt_relsz, dynsym);
#endif

    for (size_t i = 0; i < plt_relsz; i++) {
        uint32_t idx = plt_rela[i].r_info >> 32;
        void** addr = (void**)( (char*)load_addr + plt_rela[i].r_offset );
        auto name = &strtab[dynsym[idx].st_name];
        if (hook_funcs.count(name)) {
            auto pagesize = (size_t)sysconf(_SC_PAGE_SIZE);
            auto base = (void*)((size_t)addr & (~(pagesize-1)));
            assert(mprotect(base, pagesize, PROT_READ | PROT_WRITE) == 0);
            *addr = hook_funcs[name];
#ifdef DEBUG
            fprintf(stderr, "%p %p %lx %x %s\n", addr, base, pagesize, idx, name);
#endif
        }
    }

    return;
}

int __libc_start_main (
        int (*main) (int, char **, char ** MAIN_AUXVEC_DECL),
        int argc, char **argv,
        __typeof (main) init,
        void (*fini) (void),
        void (*rtld_fini) (void),
        void *stack_end) {

    logger = fdopen(atoi(getenv("LOGGER_FD")), "a");
    setvbuf(logger, NULL, _IONBF, 0);
    load_config(getenv("SANDBOX_CONFIG"));

    void* libc_handle = dlopen("libc.so.6", RTLD_LAZY);
    auto real = (decltype(__libc_start_main)*)dlsym(libc_handle, "__libc_start_main");

#define HOOK_FUNC_ADD(name) \
    hook_funcs[ #name ] = (void*)& hook_ ## name;
    DO_HOOKS(HOOK_FUNC_ADD);

    patch_got();

#ifdef DEBUG
    asm("int3");
#endif

    return real(main, argc, argv, init, fini, rtld_fini, stack_end);
}
