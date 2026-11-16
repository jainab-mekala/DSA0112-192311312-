// Q6 - Growth investigation + alternative designs: 10,000,000 calls
//   friend (inline) | friend (NOT inline) | member function | getters
// Compile TWICE and keep both tables:
//     g++ -std=c++11 -O0 q6_inline_benchmark.cpp -o bench_O0.exe
//     g++ -std=c++11 -O2 q6_inline_benchmark.cpp -o bench_O2.exe
// (In Dev-C++: Tools > Compiler Options > Settings > Code Generation > Optimization level)
#include <iostream>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

class Time {
    int h, m;
public:
    Time() : h(0), m(0) {}
    Time(int hh, int mm) : h(hh), m(mm) {}

    int getH() const { return h; }                          // getters (defined in class => inline)
    int getM() const { return m; }

    int minutesTo(const Time& b) const                      // member version
    { int x = h * 60 + m, y = b.h * 60 + b.m; return x > y ? x - y : y - x; }

    friend int minutesBetweenInline(const Time& a, const Time& b);
    friend int minutesBetweenNoInline(const Time& a, const Time& b);
};

inline int minutesBetweenInline(const Time& a, const Time& b)       // asks the compiler to inline
{ int x = a.h * 60 + a.m, y = b.h * 60 + b.m; return x > y ? x - y : y - x; }

NOINLINE int minutesBetweenNoInline(const Time& a, const Time& b)   // forced real call
{ int x = a.h * 60 + a.m, y = b.h * 60 + b.m; return x > y ? x - y : y - x; }

inline int minutesBetweenGetters(const Time& a, const Time& b)      // public getters
{ int x = a.getH() * 60 + a.getM(), y = b.getH() * 60 + b.getM(); return x > y ? x - y : y - x; }

volatile long long sink = 0;
const int CALLS = 10000000;
const int POOL  = 1024;
Time pool[POOL];

double ms(clock_t a, clock_t b) { return 1000.0 * (b - a) / CLOCKS_PER_SEC; }

int main()
{
    for (int i = 0; i < POOL; ++i) pool[i] = Time((i * 7) % 24, (i * 13) % 60);

    long long total;
    clock_t t0, t1;

    total = 0; t0 = clock();
    for (int i = 0; i < CALLS; ++i) total += minutesBetweenInline(pool[i & (POOL-1)], pool[(i * 5 + 3) & (POOL-1)]);
    t1 = clock(); sink = total;
    std::cout << "friend, inline      : " << ms(t0, t1) << " ms   (checksum " << total << ")\n";

    total = 0; t0 = clock();
    for (int i = 0; i < CALLS; ++i) total += minutesBetweenNoInline(pool[i & (POOL-1)], pool[(i * 5 + 3) & (POOL-1)]);
    t1 = clock(); sink = total;
    std::cout << "friend, not inline  : " << ms(t0, t1) << " ms   (checksum " << total << ")\n";

    total = 0; t0 = clock();
    for (int i = 0; i < CALLS; ++i) total += pool[i & (POOL-1)].minutesTo(pool[(i * 5 + 3) & (POOL-1)]);
    t1 = clock(); sink = total;
    std::cout << "member minutesTo    : " << ms(t0, t1) << " ms   (checksum " << total << ")\n";

    total = 0; t0 = clock();
    for (int i = 0; i < CALLS; ++i) total += minutesBetweenGetters(pool[i & (POOL-1)], pool[(i * 5 + 3) & (POOL-1)]);
    t1 = clock(); sink = total;
    std::cout << "public getters      : " << ms(t0, t1) << " ms   (checksum " << total << ")\n";

    std::cout << "\nAll four checksums must be identical (same answers, different call mechanism).\n";
    return 0;
}
