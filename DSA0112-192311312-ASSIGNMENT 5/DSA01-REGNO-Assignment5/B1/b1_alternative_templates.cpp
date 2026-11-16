// B1 - Alternative concept: compile-time polymorphism (templates) with ONE CONTAINER PER TYPE,
// instead of virtual functions through a base-class pointer in one list.
// Shows: time per bill (no virtual call), space (one container per type), and what is lost
// when all bills must stay in one list (arrival order, a single loop, one container to pass around).
#include <iostream>
#include <vector>
#include <ctime>

struct OutPatientBill { double c, t;        double total() const { return c + t; } };
struct InPatientBill  { double d, r, p, m;  double total() const { return d * r + p + m; } };
struct DayCareBill    { double pk;          double total() const { return pk; } };
struct EmergencyBill  { double b, h, r;     double total() const { return b + h * r; } };

template <typename T>
double sumTotals(const std::vector<T>& v)                  // compile-time polymorphism: any T with total()
{
    double s = 0;
    for (std::size_t i = 0; i < v.size(); ++i) s += v[i].total();     // direct, inlinable call
    return s;
}

// polymorphic design for comparison
struct Bill { virtual ~Bill() = default; virtual double total() const = 0; };
struct OPv : Bill { double c, t; OPv(double c_, double t_) : c(c_), t(t_) {} double total() const override { return c + t; } };
struct IPv : Bill { double d, r, p, m; IPv(double d_, double r_, double p_, double m_) : d(d_), r(r_), p(p_), m(m_) {} double total() const override { return d * r + p + m; } };
struct DCv : Bill { double pk; explicit DCv(double p) : pk(p) {} double total() const override { return pk; } };
struct ERv : Bill { double b, h, r; ERv(double b_, double h_, double r_) : b(b_), h(h_), r(r_) {} double total() const override { return b + h * r; } };

volatile double sink = 0;

int main()
{
    // ---- what is lost: arrival order ----
    std::cout << "Bills arrive in this order: OP, IP, OP, DC, ER, IP\n";
    std::cout << "one list (virtual)       : processed in arrival order OP IP OP DC ER IP\n";
    std::cout << "container per type       : processed as OP OP | IP IP | DC | ER  (arrival order is lost)\n\n";

    const int N = 1000000, ROUNDS = 20;
    std::vector<OutPatientBill> op; std::vector<InPatientBill> ip; std::vector<DayCareBill> dc; std::vector<EmergencyBill> er;
    std::vector<Bill*> poly;
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {
            case 0: { OutPatientBill b = { 500, 1200.0 + i % 50 }; op.push_back(b); poly.push_back(new OPv(b.c, b.t)); break; }
            case 1: { InPatientBill b = { 3, 2500, 40000, 5000.0 + i % 50 }; ip.push_back(b); poly.push_back(new IPv(b.d, b.r, b.p, b.m)); break; }
            case 2: { DayCareBill b = { 25000.0 + i % 50 }; dc.push_back(b); poly.push_back(new DCv(b.pk)); break; }
            default: { EmergencyBill b = { 3000, 2.5, 800.0 + i % 50 }; er.push_back(b); poly.push_back(new ERv(b.b, b.h, b.r)); break; }
        }
    }
    std::cout << "space: sizeof(OutPatientBill) = " << sizeof(OutPatientBill) << ", sizeof(OPv) = " << sizeof(OPv)
              << "  (virtual objects also pay for the vptr and a pointer in the list)\n\n";

    double a = 0, b = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) a += sumTotals(op) + sumTotals(ip) + sumTotals(dc) + sumTotals(er);
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (std::size_t i = 0; i < poly.size(); ++i) b += poly[i]->total();
    clock_t t2 = clock();
    sink = a + b;
    std::cout << N << " bills x " << ROUNDS << " rounds\n";
    std::cout << "templates, 4 containers : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "virtual, 1 list         : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "same total? " << (a == b ? "yes" : "NO") << " (" << a << ")\n";
    for (std::size_t i = 0; i < poly.size(); ++i) delete poly[i];
    return 0;
}
