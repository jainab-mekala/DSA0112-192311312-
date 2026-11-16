// B3.1 - Correctness on small data: the same 10 bills totalled with
//   (a) virtual functions (class hierarchy)   (b) ONE Bill struct with a type field and a switch.
// Both must agree for every bill and for the day's total.
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>

// ---------------- (a) virtual functions ----------------
class Bill {
public:
    virtual ~Bill() = default;
    virtual double total() const = 0;
};
class OutPatientBill : public Bill { double c_, t_;
public: OutPatientBill(double c, double t) : c_(c), t_(t) {}
        double total() const override { return c_ + t_; } };
class InPatientBill : public Bill { int d_; double r_, p_, m_;
public: InPatientBill(int d, double r, double p, double m) : d_(d), r_(r), p_(p), m_(m) {}
        double total() const override { return d_ * r_ + p_ + m_; } };
class DayCareBill : public Bill { double pk_;
public: explicit DayCareBill(double pk) : pk_(pk) {}
        double total() const override { return pk_; } };
class EmergencyBill : public Bill { double b_, h_, r_;
public: EmergencyBill(double b, double h, double r) : b_(b), h_(h), r_(r) {}
        double total() const override { return b_ + h_ * r_; } };

// ---------------- (b) struct + switch ----------------
struct BillS {
    enum Type { OP, IP, DC, ER } type;
    double a, b, c, d;
};
double totalS(const BillS& x)
{
    switch (x.type) {
        case BillS::OP: return x.a + x.b;
        case BillS::IP: return x.a * x.b + x.c + x.d;
        case BillS::DC: return x.a;
        case BillS::ER: return x.a + x.b * x.c;
    }
    return 0;
}

int main()
{
    // the same ten bills in both representations
    Bill* v[10] = {
        new OutPatientBill(500, 1200),        new InPatientBill(3, 2500, 40000, 5000),
        new DayCareBill(25000),               new EmergencyBill(3000, 2.5, 800),
        new OutPatientBill(300, 0),           new InPatientBill(1, 4000, 0, 750),
        new DayCareBill(18000),               new EmergencyBill(2500, 1, 1000),
        new OutPatientBill(700, 2300),        new InPatientBill(7, 3000, 120000, 15000)
    };
    BillS s[10] = {
        { BillS::OP, 500, 1200, 0, 0 },       { BillS::IP, 3, 2500, 40000, 5000 },
        { BillS::DC, 25000, 0, 0, 0 },        { BillS::ER, 3000, 2.5, 800, 0 },
        { BillS::OP, 300, 0, 0, 0 },          { BillS::IP, 1, 4000, 0, 750 },
        { BillS::DC, 18000, 0, 0, 0 },        { BillS::ER, 2500, 1, 1000, 0 },
        { BillS::OP, 700, 2300, 0, 0 },       { BillS::IP, 7, 3000, 120000, 15000 }
    };

    double sumV = 0, sumS = 0;
    bool allAgree = true;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << " #   virtual      switch     agree\n";
    for (int i = 0; i < 10; ++i) {
        double a = v[i]->total(), b = totalS(s[i]);
        bool same = std::fabs(a - b) < 1e-9;
        allAgree = allAgree && same;
        sumV += a; sumS += b;
        std::cout << std::setw(2) << i + 1 << std::setw(12) << a << std::setw(12) << b << "     " << (same ? "yes" : "NO") << '\n';
    }
    std::cout << "day total: virtual " << sumV << "  switch " << sumS << "  -> "
              << ((allAgree && std::fabs(sumV - sumS) < 1e-9) ? "BOTH AGREE" : "MISMATCH") << '\n';
    // hand check of the day total:
    // 1700 + 52500 + 25000 + 5000 + 300 + 4750 + 18000 + 3500 + 3000 + 156000 = 269750
    std::cout << "hand-computed day total: 269750.00\n";
    for (int i = 0; i < 10; ++i) delete v[i];
    return 0;
}
