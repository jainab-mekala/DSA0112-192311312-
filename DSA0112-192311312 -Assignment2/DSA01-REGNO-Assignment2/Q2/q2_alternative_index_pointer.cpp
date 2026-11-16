// Q2 - Alternative concept: return an index or a pointer instead of int&.
// Time: O(n) for all three (one scan). Space: O(1) for all three.
#include <iostream>
#include <vector>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

NOINLINE int& highestRef(int a[], int n)  { int k = 0; for (int i = 1; i < n; ++i) if (a[i] > a[k]) k = i; return a[k]; }
NOINLINE int  highestIndex(const int a[], int n) { int k = 0; for (int i = 1; i < n; ++i) if (a[i] > a[k]) k = i; return k; }
NOINLINE int* highestPtr(int a[], int n)  { int k = 0; for (int i = 1; i < n; ++i) if (a[i] > a[k]) k = i; return &a[k]; }

volatile long long sink = 0;

int main()
{
    int marks[6] = {56, 78, 91, 64, 85, 72};

    // how the CALLING code looks for each design
    highestRef(marks, 6) += 2;                  // reference : call used directly
    marks[highestIndex(marks, 6)] += 2;         // index     : caller indexes the array
    *highestPtr(marks, 6) += 2;                 // pointer   : caller dereferences
    std::cout << "highest after three +2 moderations: " << marks[2] << " (expected 97)\n\n";

    // safety when storage grows
    std::vector<int> v;
    v.push_back(56); v.push_back(78); v.push_back(91);
    int idx  = highestIndex(&v[0], (int)v.size());
    int* ptr = highestPtr(&v[0], (int)v.size());
    const int* oldAddr = ptr;
    std::size_t oldCap = v.capacity();
    while (v.capacity() == oldCap) v.push_back(1);   // grow until the buffer is reallocated
    std::cout << "after growth: index still valid -> v[" << idx << "] = " << v[idx] << '\n';
    std::cout << "after growth: pointer still points into the live buffer? "
              << (oldAddr == &v[idx] ? "yes (buffer did not move)" : "NO - buffer moved, old pointer is dangling (do not dereference)") << "\n\n";

    // timing: all three are O(n)
    const int N = 1000000, REPS = 200;
    std::vector<int> big(N);
    for (int i = 0; i < N; ++i) big[i] = (int)(((long long)i * 7919) % 100003);
    long long total = 0;
    clock_t t0, t1;

    t0 = clock();
    for (int r = 0; r < REPS; ++r) { big[r] = r; total += highestRef(&big[0], N); }
    t1 = clock();
    std::cout << "int&  version : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    t0 = clock();
    for (int r = 0; r < REPS; ++r) { big[r] = r; total += big[highestIndex(&big[0], N)]; }
    t1 = clock();
    std::cout << "index version : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    t0 = clock();
    for (int r = 0; r < REPS; ++r) { big[r] = r; total += *highestPtr(&big[0], N); }
    t1 = clock();
    std::cout << "int*  version : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    sink = total;
    return 0;
}
