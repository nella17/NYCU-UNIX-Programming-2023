#include <bits/stdc++.h>
extern "C" {
#include "sort.c"
}

std::mt19937 rng((int)std::chrono::steady_clock::now().time_since_epoch().count());

bool test(int size, int argc) {
    long* ary = new long[size]();
    std::iota(ary, ary+size, 0);
    std::shuffle(ary, ary+size, rng);
    if (argc > 1)
        for (int i = 0; i < size; i++)
            printf("%ld%c", ary[i], " \n"[i+1==size]);
    sort(ary, size);
    if (argc > 1)
        for (int i = 0; i < size; i++)
            printf("%ld%c", ary[i], " \n"[i+1==size]);
    bool can = true;
    for (int i = 0; i < size; i++)
        if (ary[i] != i)
            can = false;
    delete [] ary;
    return can;
}

int main(int argc) {
    if (test(16, argc)) puts("16");
    if (test(1e6, argc)) puts("1e6");
}
