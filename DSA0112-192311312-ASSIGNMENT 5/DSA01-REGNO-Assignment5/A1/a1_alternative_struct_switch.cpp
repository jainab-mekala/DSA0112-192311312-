// A1 - Alternative concept: ONE Bill structure with a type field and a switch in total().
// Compares the time to total n bills, the space per bill and the code changed for a new type.
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

// ------------------------- alternative: struct + switch -------------------------
struct BillS {
    enum Type { OP, IP, DC, ER };                // new bill type -> edit #1: add an enumerator
    Type        type;
    std::string patient;
    double      a, b, c, d;                      // meaning depends on type; unused fields in some types
};

double totalS(const BillS& x)
{
    switch (x.type) {                            // new bill type -> edit #2: add a case here
        case BillS::OP: return x.a + x.b;                        // consultation + tests
        case BillS::IP: return x.a * x.b + x.c + x.d;            // days*perDay + procedures + medicines
        case BillS::DC: return x.a;                              // package price
        case BillS::ER: return x.a + x.b * x.c;                  // base + hours*rate
    }
    return 0;
}
// every other function that looks at the type (print, type name, validation ...) has its own
// switch and must ALSO be edited for a new type -> edits #3, #4, ...

// ------------------------- hierarchy -------------------------
class Bill {
protected: std::string patient_;
public:
    explicit Bill(const std::string& p) : patient_(p) {}
    virtual ~Bill() = default;
    virtual double total() const = 0;
};
class OutPatientBill : public Bill { double c_, t_;
public: OutPatientBill(const std::string& p, double c, double t) : Bill(p), c_(c), t_(t) {}
        double total() const override { return c_ + t_; } };
class InPatientBill : public Bill { double d_, r_, p_, m_;
public: InPatientBill(const std::string& p, double d, double r, double pr, double m) : Bill(p), d_(d), r_(r), p_(pr), m_(m) {}
        double total() const override { return d_ * r_ + p_ + m_; } };
class DayCareBill : public Bill { double pk_;
public: DayCareBill(const std::string& p, double pk) : Bill(p), pk_(pk) {}
        double total() const override { return pk_; } };
class EmergencyBill : public Bill { double b_, h_, r_;
public: EmergencyBill(const std::string& p, double b, double h, double r) : Bill(p), b_(b), h_(h), r_(r) {}
        double total() const override { return b_ + h_ * r_; } };

volatile double sink = 0;

int main()
{
    std::cout << "sizeof(BillS)           = " << sizeof(BillS) << " bytes (same for every type)\n";
    std::cout << "sizeof(OutPatientBill)  = " << sizeof(OutPatientBill) << '\n';
    std::cout << "sizeof(InPatientBill)   = " << sizeof(InPatientBill) << '\n';
    std::cout << "sizeof(DayCareBill)     = " << sizeof(DayCareBill) << '\n';
    std::cout << "sizeof(EmergencyBill)   = " << sizeof(EmergencyBill) << "  (each includes one vtable pointer)\n\n";

    const int N = 1000, ROUNDS = 20000;                    // 20,000,000 total() calls each
    std::vector<BillS> flat;
    std::vector<Bill*> poly;
    for (int i = 0; i < N; ++i) {
        std::string p = "P" + std::to_string(i);
        switch (i % 4) {
            case 0: flat.push_back({BillS::OP, p, 500, 1200.0 + i, 0, 0});            poly.push_back(new OutPatientBill(p, 500, 1200 + i)); break;
            case 1: flat.push_back({BillS::IP, p, 3, 2500, 40000, 5000.0 + i});     poly.push_back(new InPatientBill(p, 3, 2500, 40000, 5000 + i)); break;
            case 2: flat.push_back({BillS::DC, p, 25000.0 + i, 0, 0, 0});           poly.push_back(new DayCareBill(p, 25000.0 + i)); break;
            default: flat.push_back({BillS::ER, p, 3000, 2.5, 800.0 + i, 0});       poly.push_back(new EmergencyBill(p, 3000, 2.5, 800.0 + i)); break;
        }
    }

    double a = 0, b = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) a += totalS(flat[i]);
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) b += poly[i]->total();
    clock_t t2 = clock();
    sink = a + b;

    std::cout << "switch  : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "virtual : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "same total? " << (a == b ? "yes" : "NO") << " (" << a << ")\n";
    for (int i = 0; i < N; ++i) delete poly[i];
    return 0;
}
