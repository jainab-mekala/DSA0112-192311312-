// B3.3 - Cost of exceptions: validate 1,000,000 bills when errors are reported by
// return codes and when they are reported by exceptions, with 0%, 1% and 50% invalid bills.
// COMPILE WITH -O2.  A bill is invalid when its charge is negative.
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

struct Item { double charge; };

enum Status { OK, NEGATIVE };

NOINLINE Status validateCode(const Item& it, double& out)        // error as a return value
{
    if (it.charge < 0) return NEGATIVE;
    out = it.charge;
    return OK;
}

NOINLINE double validateEx(const Item& it)                       // error as an exception
{
    if (it.charge < 0) throw std::invalid_argument("negative charge");
    return it.charge;
}

volatile double sink = 0;

int main()
{
    const int N = 1000000;
    const int percents[3] = { 0, 1, 50 };
    std::cout << std::fixed << std::setprecision(3);
    std::cout << std::setw(10) << "invalid" << std::setw(16) << "return codes" << std::setw(16) << "exceptions"
              << std::setw(14) << "ratio" << std::setw(20) << "extra ns / error" << '\n';

    for (int k = 0; k < 3; ++k) {
        int pct = percents[k];
        std::vector<Item> items(N);
        int bad = 0;
        for (int i = 0; i < N; ++i) {
            bool invalid = (i % 100) < pct;
            items[i].charge = invalid ? -50.0 : 100.0 + i % 50;
            if (invalid) ++bad;
        }

        double total = 0; int errA = 0, errB = 0;
        clock_t t0 = clock();
        for (int i = 0; i < N; ++i) { double v = 0; if (validateCode(items[i], v) == OK) total += v; else ++errA; }
        clock_t t1 = clock();
        for (int i = 0; i < N; ++i) {
            try { total += validateEx(items[i]); }
            catch (const std::invalid_argument&) { ++errB; }
        }
        clock_t t2 = clock();
        sink = total;

        double msA = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC, msB = 1000.0 * (t2 - t1) / CLOCKS_PER_SEC;
        std::cout << std::setw(9) << pct << "%" << std::setw(13) << msA << " ms" << std::setw(13) << msB << " ms"
                  << std::setw(13) << (msA > 0 ? msB / msA : 0.0) << "x"
                  << std::setw(20) << (bad > 0 ? 1e6 * (msB - msA) / bad : 0.0)
                  << (errA == bad && errB == bad ? "" : "   COUNT MISMATCH") << '\n';
    }
    return 0;
}
