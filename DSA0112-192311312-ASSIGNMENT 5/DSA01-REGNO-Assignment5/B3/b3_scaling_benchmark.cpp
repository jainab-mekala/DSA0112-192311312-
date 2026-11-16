// B3.2 - Increase the data size: virtual functions vs a struct with a switch for
// 1,000 / 1,000,000 / 10,000,000 bills. COMPILE WITH -O2:
//     g++ -std=c++17 -O2 -Wall -Wextra b3_scaling_benchmark.cpp -o b3_scaling.exe
// Peak memory is about 450 MB at 10,000,000 bills. If your PC has little RAM,
// delete 10000000 from SIZES (and say so in your report).
// Only the traversal (summing total()) is timed, not the construction.
#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>

// ---------------- virtual design (bills without strings, to keep memory reasonable) ----------------
class Bill { public: virtual ~Bill() = default; virtual double total() const = 0; };
struct OPv : Bill { double c, t;        OPv(double c_, double t_) : c(c_), t(t_) {}                         double total() const override { return c + t; } };
struct IPv : Bill { double d, r, p, m;  IPv(double d_, double r_, double p_, double m_) : d(d_), r(r_), p(p_), m(m_) {} double total() const override { return d * r + p + m; } };
struct DCv : Bill { double pk;          explicit DCv(double p) : pk(p) {}                                  double total() const override { return pk; } };
struct ERv : Bill { double b, h, r;     ERv(double b_, double h_, double r_) : b(b_), h(h_), r(r_) {}      double total() const override { return b + h * r; } };

// ---------------- struct + switch design ----------------
struct BillS { int type; double a, b, c, d; };           // 0=OP 1=IP 2=DC 3=ER
inline double totalS(const BillS& x)
{
    switch (x.type) {
        case 0: return x.a + x.b;
        case 1: return x.a * x.b + x.c + x.d;
        case 2: return x.a;
        default: return x.a + x.b * x.c;
    }
}

volatile double sink = 0;

int main()
{
    const long long SIZES[3] = { 1000, 1000000, 10000000 };
    std::cout << std::fixed << std::setprecision(3);
    std::cout << std::setw(12) << "bills" << std::setw(8) << "reps" << std::setw(16) << "virtual (ms)"
              << std::setw(16) << "switch (ms)" << std::setw(14) << "virtual ns/b" << std::setw(14) << "switch ns/b" << "  same?\n";

    for (int k = 0; k < 3; ++k) {
        const long long N = SIZES[k];
        const long long reps = (20000000 / N) > 0 ? 20000000 / N : 1;      // about 20M calls in total

        double virtualSum = 0, switchSum = 0, msV = 0, msS = 0;

        {   // ----- virtual -----
            std::vector<OPv> op; std::vector<IPv> ip; std::vector<DCv> dc; std::vector<ERv> er;
            op.reserve(N / 4 + 1); ip.reserve(N / 4 + 1); dc.reserve(N / 4 + 1); er.reserve(N / 4 + 1);   // no reallocation: pointers stay valid
            std::vector<Bill*> bills; bills.reserve(N);
            for (long long i = 0; i < N; ++i) {
                switch (i % 4) {
                    case 0: op.push_back(OPv(500, 1200.0 + i % 50)); bills.push_back(&op.back()); break;
                    case 1: ip.push_back(IPv(3, 2500, 40000, 5000.0 + i % 50)); bills.push_back(&ip.back()); break;
                    case 2: dc.push_back(DCv(25000.0 + i % 50)); bills.push_back(&dc.back()); break;
                    default: er.push_back(ERv(3000, 2.5, 800.0 + i % 50)); bills.push_back(&er.back()); break;
                }
            }
            clock_t t0 = clock();
            for (long long r = 0; r < reps; ++r) { double s = 0; for (long long i = 0; i < N; ++i) s += bills[i]->total(); virtualSum = s; }
            clock_t t1 = clock();
            msV = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        }
        {   // ----- switch -----
            std::vector<BillS> bills; bills.reserve(N);
            for (long long i = 0; i < N; ++i) {
                BillS b;
                switch (i % 4) {
                    case 0: b.type = 0; b.a = 500; b.b = 1200.0 + i % 50; b.c = 0; b.d = 0; break;
                    case 1: b.type = 1; b.a = 3; b.b = 2500; b.c = 40000; b.d = 5000.0 + i % 50; break;
                    case 2: b.type = 2; b.a = 25000.0 + i % 50; b.b = 0; b.c = 0; b.d = 0; break;
                    default: b.type = 3; b.a = 3000; b.b = 2.5; b.c = 800.0 + i % 50; b.d = 0; break;
                }
                bills.push_back(b);
            }
            clock_t t0 = clock();
            for (long long r = 0; r < reps; ++r) { double s = 0; for (long long i = 0; i < N; ++i) s += totalS(bills[i]); switchSum = s; }
            clock_t t1 = clock();
            msS = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
        }
        sink = virtualSum + switchSum;
        double calls = double(N) * double(reps);
        std::cout << std::setw(12) << N << std::setw(8) << reps << std::setw(16) << msV << std::setw(16) << msS
                  << std::setw(14) << 1e6 * msV / calls << std::setw(14) << 1e6 * msS / calls
                  << "  " << (virtualSum == switchSum ? "yes" : "NO") << '\n';
    }
    std::cout << "\n(\"reps\" repeats the traversal so every row performs about 20,000,000 total() calls)\n";
    return 0;
}
