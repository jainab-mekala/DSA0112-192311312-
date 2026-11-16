// A2 - Alternative concept: total() is NOT virtual; the program finds the right function by
// testing the type of each bill with dynamic_cast (up to k = 4 casts per bill).
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

// (A) dynamic_cast design. Bill still needs ONE virtual function (the destructor), otherwise
//     dynamic_cast does not compile - so the vtable pointer is still in every object!
struct BillD { virtual ~BillD() = default; };
struct OPd : BillD { double c, t; OPd(double c_, double t_) : c(c_), t(t_) {} double total() const { return c + t; } };
struct IPd : BillD { double d, r, p, m; IPd(double d_, double r_, double p_, double m_) : d(d_), r(r_), p(p_), m(m_) {} double total() const { return d * r + p + m; } };
struct DCd : BillD { double pk; explicit DCd(double p) : pk(p) {} double total() const { return pk; } };
struct ERd : BillD { double b, h, r; ERd(double b_, double h_, double r_) : b(b_), h(h_), r(r_) {} double total() const { return b + h * r; } };

double totalByCast(const BillD* x)
{
    if (const OPd* p = dynamic_cast<const OPd*>(x)) return p->total();     // cast 1
    if (const IPd* p = dynamic_cast<const IPd*>(x)) return p->total();     // cast 2
    if (const DCd* p = dynamic_cast<const DCd*>(x)) return p->total();     // cast 3
    if (const ERd* p = dynamic_cast<const ERd*>(x)) return p->total();     // cast 4
    return 0;
}

// (B) virtual design
struct BillV { virtual ~BillV() = default; virtual double total() const = 0; };
struct OPv : BillV { double c, t; OPv(double c_, double t_) : c(c_), t(t_) {} double total() const override { return c + t; } };
struct IPv : BillV { double d, r, p, m; IPv(double d_, double r_, double p_, double m_) : d(d_), r(r_), p(p_), m(m_) {} double total() const override { return d * r + p + m; } };
struct DCv : BillV { double pk; explicit DCv(double p) : pk(p) {} double total() const override { return pk; } };
struct ERv : BillV { double b, h, r; ERv(double b_, double h_, double r_) : b(b_), h(h_), r(r_) {} double total() const override { return b + h * r; } };

// (C) no polymorphism at all: no vptr
struct OPplain { double c, t; };

volatile double sink = 0;

int main()
{
    std::cout << "sizeof(OPplain, no vptr)   = " << sizeof(OPplain) << " bytes\n";
    std::cout << "sizeof(OPd, dynamic_cast)  = " << sizeof(OPd) << " bytes (vptr present because ~BillD is virtual)\n";
    std::cout << "sizeof(OPv, virtual total) = " << sizeof(OPv) << " bytes\n\n";

    const int N = 1000, ROUNDS = 10000;                    // 10,000,000 total() calls each
    std::vector<BillD*> cast;
    std::vector<BillV*> virt;
    for (int i = 0; i < N; ++i) {
        switch (i % 4) {                                   // equal mix: average 2.5 casts, worst case 4
            case 0: cast.push_back(new OPd(500, 1200 + i));         virt.push_back(new OPv(500, 1200 + i)); break;
            case 1: cast.push_back(new IPd(3, 2500, 40000, 5000));  virt.push_back(new IPv(3, 2500, 40000, 5000)); break;
            case 2: cast.push_back(new DCd(25000.0 + i));           virt.push_back(new DCv(25000.0 + i)); break;
            default: cast.push_back(new ERd(3000, 2.5, 800));       virt.push_back(new ERv(3000, 2.5, 800)); break;
        }
    }
    double a = 0, b = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) a += totalByCast(cast[i]);
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) b += virt[i]->total();
    clock_t t2 = clock();
    sink = a + b;
    std::cout << "dynamic_cast chain : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "one virtual call   : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "same total? " << (a == b ? "yes" : "NO") << " (" << a << ")\n";
    for (int i = 0; i < N; ++i) { delete cast[i]; delete virt[i]; }
    return 0;
}
