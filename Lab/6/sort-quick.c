void sort(long *numbers, int n);
void impl(long* begin, long* end);

#ifdef DEBUG
#include <cstdio>
long* gn;
#endif

void sort(long *numbers, int n) {
#ifdef DEBUG
    gn = numbers;
#endif
    impl(numbers, numbers + n - 1);
}


void impl(long* begin, long* end) {
#ifdef DEBUG
    // printf("%ld %ld %ld\n", end - begin, begin - gn, end - gn);
#endif

    long* mid = begin + (end - begin);
    int a = *begin < *mid, b = *mid < *end, c = *begin < *end;
    long* ppv =
        (a ^ b) ? mid :
        (a ^ !c) ? begin :
                    end;
	long pivot = *ppv;
    *ppv = *begin;

	long* i = begin+1;
	long* j = end;
	while (i <= j) {
		while (i <= j && *i <= pivot)
			i++;
		while (i <= j && *j > pivot)
			j--;
		if (i < j) {
            long x = *i, y = *j;
            *i = y; *j = x;
		}
	}

    *begin = *j;
    *j = pivot;
    if (begin < j-1)
        impl(begin, j-1);
    if (j+1 < end)
        impl(j+1, end);
}

