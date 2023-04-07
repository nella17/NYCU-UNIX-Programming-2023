extern "C"
int __libc_start_main (
    int (*main) (int, char **, char ** MAIN_AUXVEC_DECL),
    int argc, char **argv,
    __typeof (main) init,
    void (*fini) (void),
    void (*rtld_fini) (void),
    void *stack_end);
