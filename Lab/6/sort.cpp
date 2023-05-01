extern "C"
void sort(long *numbers, int n);

extern "C"
void merge(long* begin, long* end);

void sort(long *numbers, int n) {
    merge(numbers, numbers + n);
}

void merge(long* begin, long* end) {
    int n = end - begin;
    if (n == 1) return;

    long* mid = begin + n / 2;
    merge(begin, mid);
    merge(mid, end);

    long ary[n];

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

#ifdef DEBUG
#include <bits/stdc++.h>

std::mt19937 rng((int)std::chrono::steady_clock::now().time_since_epoch().count());

int main() {
    int size = 10;
    long* ary = new long[size]();
    std::iota(ary, ary+size, 0);
    std::shuffle(ary, ary+size, rng);
    for (int i = 0; i < size; i++)
        printf("%ld%c", ary[i], " \n"[i+1==size]);
    sort(ary, size);
    for (int i = 0; i < size; i++)
        printf("%ld%c", ary[i], " \n"[i+1==size]);
}
#endif
