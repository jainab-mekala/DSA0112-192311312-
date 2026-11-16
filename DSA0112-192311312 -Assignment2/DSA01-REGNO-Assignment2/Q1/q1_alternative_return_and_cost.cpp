// Q1 - Alternative concept: return the new fee instead of changing it in place,
//      and the cost of passing a large object by value / reference / const reference.
#include <iostream>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

// ---- Design decision: in place vs return ---------------------------------
long long applyConcessionReturn(long long fee, int percent) { return fee - fee * percent / 100; }
void applyConcessionInPlace(long long& fee, int percent)    { fee -= fee * percent / 100; }

// ---- A large object to show the copying cost -------------------------------
struct Student {
    char   name[100];
    int    regNo;
    long long fee;
    int    marks[200];
    double extra[50];
};

NOINLINE long long feeByValue(Student s)           { return s.fee; }
NOINLINE long long feeByRef(Student& s)            { return s.fee; }
NOINLINE long long feeByConstRef(const Student& s) { return s.fee; }

volatile long long sink = 0;

int main()
{
    // 1. several concessions: chained returns vs repeated in-place calls
    long long fee = 45001;
    long long chained = applyConcessionReturn(applyConcessionReturn(applyConcessionReturn(fee, 10), 15), 25);
    long long inPlace = fee;
    applyConcessionInPlace(inPlace, 10);
    applyConcessionInPlace(inPlace, 15);
    applyConcessionInPlace(inPlace, 25);
    std::cout << "original fee            : " << fee << '\n';
    std::cout << "returned (chained)      : " << chained << '\n';
    std::cout << "in place (3 calls)      : " << inPlace << '\n';
    std::cout << "original still available with return version: " << fee << "\n\n";

    long long otherOrder = applyConcessionReturn(applyConcessionReturn(applyConcessionReturn(fee, 25), 15), 10);
    std::cout << "same concessions, order 25,15,10 : " << otherOrder
              << "  (integer truncation can make order matter)\n\n";

    // 2. space copied per call
    std::cout << "sizeof(long long) = " << sizeof(long long) << " bytes\n";
    std::cout << "sizeof(Student)   = " << sizeof(Student) << " bytes\n\n";

    // 3. measured time
    const int CALLS = 2000000;
    static Student s;          // static: keeps the big object off the stack
    s.fee = 1;
    clock_t t0, t1;
    long long total = 0;

    t0 = clock();
    for (int i = 0; i < CALLS; ++i) { s.fee = i; total += feeByValue(s); }
    t1 = clock();
    std::cout << "Student by value      : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    t0 = clock();
    for (int i = 0; i < CALLS; ++i) { s.fee = i; total += feeByRef(s); }
    t1 = clock();
    std::cout << "Student by reference  : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    t0 = clock();
    for (int i = 0; i < CALLS; ++i) { s.fee = i; total += feeByConstRef(s); }
    t1 = clock();
    std::cout << "Student by const ref  : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    sink = total;
    std::cout << "(checksum " << sink << ")\n";
    return 0;
}
