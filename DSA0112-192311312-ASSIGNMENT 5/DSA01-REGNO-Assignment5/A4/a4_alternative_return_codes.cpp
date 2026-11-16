// A4 - Alternative concept: report errors with return codes (enum Status) instead of exceptions.
// Shows: (1) the extra code needed to pass every error back, (2) time on the normal path
// and on the error path for both designs.
#include <iostream>
#include <stdexcept>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

enum Status { OK, NEGATIVE_CHARGE, CLAIM_ABOVE_COVER };

const char* describe(Status s)
{
    switch (s) {
        case OK: return "ok";
        case NEGATIVE_CHARGE: return "negative charge";
        case CLAIM_ABOVE_COVER: return "claim above cover";
    }
    return "?";
}

// ---------------- return-code design ----------------
Status checkCharge(double v, double& out) { if (v < 0) return NEGATIVE_CHARGE; out = v; return OK; }
Status checkClaim(double billed, double cover, double& out)
{
    if (billed > cover) return CLAIM_ABOVE_COVER;
    out = billed;
    return OK;
}
// EVERY caller in the chain must test and forward the status (more lines at each level):
NOINLINE Status settleCode(double charge, double cover, double& paid)
{
    double c = 0;
    Status s = checkCharge(charge, c);
    if (s != OK) return s;                              // forward
    s = checkClaim(c, cover, paid);
    if (s != OK) return s;                              // forward
    return OK;
}

// ---------------- exception design ----------------
double checkChargeEx(double v) { if (v < 0) throw std::invalid_argument("negative charge"); return v; }
double checkClaimEx(double billed, double cover) { if (billed > cover) throw std::out_of_range("claim above cover"); return billed; }
NOINLINE double settleEx(double charge, double cover)             // no checking code at all: errors travel by themselves
{
    return checkClaimEx(checkChargeEx(charge), cover);
}

volatile double sink = 0;

int main()
{
    const int N = 1000000;
    double total = 0;
    clock_t t0, t1;

    // normal path: every call succeeds
    t0 = clock();
    for (int i = 0; i < N; ++i) { double paid = 0; if (settleCode(1000 + i % 100, 50000, paid) == OK) total += paid; }
    t1 = clock();
    std::cout << "NORMAL path (1,000,000 valid calls)\n";
    std::cout << "  return codes : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    t0 = clock();
    for (int i = 0; i < N; ++i) total += settleEx(1000 + i % 100, 50000);
    t1 = clock();
    std::cout << "  exceptions   : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n\n";

    // error path: every call fails (use fewer calls for exceptions: each throw is expensive)
    const int E = 100000;
    t0 = clock();
    int failed = 0;
    for (int i = 0; i < E; ++i) { double paid = 0; if (settleCode(60000 + i % 7, 50000, paid) != OK) ++failed; }
    t1 = clock();
    std::cout << "ERROR path (" << E << " failing calls)\n";
    std::cout << "  return codes : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    t0 = clock();
    for (int i = 0; i < E; ++i) {
        try { total += settleEx(60000 + i % 7, 50000); }
        catch (const std::out_of_range&) { ++failed; }
    }
    t1 = clock();
    std::cout << "  exceptions   : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";

    sink = total;
    std::cout << "\nfailures counted: " << failed << " (expected " << 2 * E << ")\n";
    std::cout << "status for a negative charge: " << describe(NEGATIVE_CHARGE) << '\n';
    return 0;
}
