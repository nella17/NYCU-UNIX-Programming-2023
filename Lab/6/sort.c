void __attribute__ ((noinline)) sort(long *numbers, int n);
void merge(long* begin, long* end, long* ary);

typedef void (*fp)(long*, long*);

void sort(long *numbers, int n) {
    asm(
    "push   rbp\n"
    "mov    rbp,rsp\n"
    "movsxd rsi,esi\n"

    "lea    rdx,[rsi*8]\n"
    "sub    rsp,rdx\n"

    "mov    rdx,rsp\n"
    "lea    rsi,[rdi+rsi*8]\n"
    "call   merge\n"

    "leave  \n"
    );
}

void merge(long* begin, long* end, long* ary) {
    int n = end - begin;
    if (n <= 1) return;

    long* mid = begin + n / 2;
    merge(begin, mid, ary);
    merge(mid, end, ary);

    // inplace_merge(begin, mid, end);
    long *l = begin, *r = mid, *a = ary;
    while (l < mid && r < end) {
        long x = *l, y = *r;
        if (x < y)
            *(a++) = x, l++;
        else
            *(a++) = y, r++;
    }
    while (l < mid) *(a++) = *(l++);
    while (r < end) *(a++) = *(r++);

    for (int i = 0; i < n; i++)
        begin[i] = ary[i];
}
