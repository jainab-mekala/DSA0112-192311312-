// B3 - Alternative concept: bills held in std::variant<OP, IP, DC, ER> and processed with std::visit.
// Times 10,000,000 bills against the virtual version and the switch version.
// Needs C++17.  COMPILE WITH -O2.  Peak memory about 450 MB; lower N if your PC has little RAM.
#include <iostream>
#include <iomanip>
#include <variant>
#include <vector>
#include <ctime>

struct OP { double c, t;        double total() const { return c + t; } };
struct IP { double d, r, p, m;  double total() const { return d * r + p + m; } };
struct DC { double pk;          double total() const { return pk; } };
struct ER { double b, h, r;     double total() const { return b + h * r; } };
typedef std::variant<OP, IP, DC, ER> AnyBill;

struct TotalVisitor {
    template <typename T> double operator()(const T& x) const { return x.total(); }
};

// virtual
class Bill { public: virtual ~Bill() = default; virtual double total() const = 0; };
struct OPv : Bill { OP x; explicit OPv(OP v) : x(v) {} double total() const override { return x.total(); } };
struct IPv : Bill { IP x; explicit IPv(IP v) : x(v) {} double total() const override { return x.total(); } };
struct DCv : Bill { DC x; explicit DCv(DC v) : x(v) {} double total() const override { return x.total(); } };
struct ERv : Bill { ER x; explicit ERv(ER v) : x(v) {} double total() const override { return x.total(); } };

// switch
struct BillS { int type; double a, b, c, d; };
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
    const long long N = 10000000;
    const int REPS = 2;
    std::cout << "sizeof(OP)=" << sizeof(OP) << " sizeof(IP)=" << sizeof(IP) << " sizeof(DC)=" << sizeof(DC)
              << " sizeof(ER)=" << sizeof(ER) << '\n';
    std::cout << "sizeof(variant)  = " << sizeof(AnyBill) << " bytes = largest alternative (" << sizeof(IP) << ") + tag (padded)\n";
    std::cout << "sizeof(BillS)    = " << sizeof(BillS) << " bytes\n\n";

    double vs = 0, ss = 0, ps = 0;
    double msV = 0, msS = 0, msP = 0;

    {   // variant
        std::vector<AnyBill> bills; bills.reserve(N);
        for (long long i = 0; i < N; ++i) {
            switch (i % 4) {
                case 0: bills.push_back(OP{500, 1200.0 + i % 50}); break;
                case 1: bills.push_back(IP{3, 2500, 40000, 5000.0 + i % 50}); break;
                case 2: bills.push_back(DC{25000.0 + i % 50}); break;
                default: bills.push_back(ER{3000, 2.5, 800.0 + i % 50}); break;
            }
        }
        clock_t t0 = clock();
        for (int r = 0; r < REPS; ++r) { double s = 0; for (long long i = 0; i < N; ++i) s += std::visit(TotalVisitor(), bills[i]); vs = s; }
        clock_t t1 = clock();
        msP = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    }
    {   // virtual
        std::vector<OPv> op; std::vector<IPv> ip; std::vector<DCv> dc; std::vector<ERv> er;
        op.reserve(N / 4 + 1); ip.reserve(N / 4 + 1); dc.reserve(N / 4 + 1); er.reserve(N / 4 + 1);
        std::vector<Bill*> bills; bills.reserve(N);
        for (long long i = 0; i < N; ++i) {
            switch (i % 4) {
                case 0: op.push_back(OPv(OP{500, 1200.0 + i % 50})); bills.push_back(&op.back()); break;
                case 1: ip.push_back(IPv(IP{3, 2500, 40000, 5000.0 + i % 50})); bills.push_back(&ip.back()); break;
                case 2: dc.push_back(DCv(DC{25000.0 + i % 50})); bills.push_back(&dc.back()); break;
                default: er.push_back(ERv(ER{3000, 2.5, 800.0 + i % 50})); bills.push_back(&er.back()); break;
            }
        }
        clock_t t0 = clock();
        for (int r = 0; r < REPS; ++r) { double s = 0; for (long long i = 0; i < N; ++i) s += bills[i]->total(); ss = s; }
        clock_t t1 = clock();
        msV = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    }
    {   // switch
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
        for (int r = 0; r < REPS; ++r) { double s = 0; for (long long i = 0; i < N; ++i) s += totalS(bills[i]); ps = s; }
        clock_t t1 = clock();
        msS = 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
    }
    sink = vs + ss + ps;

    std::cout << std::fixed << std::setprecision(1);
    std::cout << N << " bills, " << REPS << " traversals each\n";
    std::cout << "std::variant + visit : " << msP << " ms\n";
    std::cout << "virtual functions    : " << msV << " ms\n";
    std::cout << "struct + switch      : " << msS << " ms\n";
    std::cout << "all totals equal? " << ((vs == ss && ss == ps) ? "yes" : "NO") << '\n';
    return 0;
}
