// B2 - Alternative concept: reject invalid claims with std::optional<double> (empty = rejected)
// instead of throwing. Measures 1,000,000 claims with 0%, 1% and 50% invalid claims.
// Needs C++17 (std::optional).
#include <iostream>
#include <optional>
#include <stdexcept>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

NOINLINE std::optional<double> approveOpt(double billed, double cover)
{
    if (billed < 0 || billed > cover) return std::nullopt;      // no value = rejected (reason is lost!)
    return billed;
}

NOINLINE double approveEx(double billed, double cover)
{
    if (billed < 0) throw std::invalid_argument("negative amount");
    if (billed > cover) throw std::out_of_range("exceeds cover");
    return billed;
}

volatile double sink = 0;

int main()
{
    std::cout << "sizeof(double) = " << sizeof(double) << ", sizeof(std::optional<double>) = "
              << sizeof(std::optional<double>) << " (value + 'has value' flag)\n\n";

    const int N = 1000000;
    const int invalidPercent[3] = { 0, 1, 50 };

    for (int k = 0; k < 3; ++k) {
        int pct = invalidPercent[k];
        double total = 0; int rejected = 0;

        clock_t t0 = clock();
        for (int i = 0; i < N; ++i) {
            double billed = (i % 100 < pct) ? 60000 : 1000 + i % 100;     // 60000 exceeds the cover of 50000
            std::optional<double> r = approveOpt(billed, 50000);
            if (r) total += *r; else ++rejected;
        }
        clock_t t1 = clock();
        int rejected2 = 0;
        for (int i = 0; i < N; ++i) {
            double billed = (i % 100 < pct) ? 60000 : 1000 + i % 100;
            try { total += approveEx(billed, 50000); }
            catch (const std::exception&) { ++rejected2; }
        }
        clock_t t2 = clock();
        sink = total;

        std::cout << pct << "% invalid  |  optional: " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms"
                  << "  |  exceptions: " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms"
                  << "  |  rejected " << rejected << " / " << rejected2 << '\n';
    }
    return 0;
}
