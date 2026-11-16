// Task 1 - Alternative concept: ONE Staff class with a type field and a switch inside pay().
// Compares: time per pay() call, space per object, and places to change for a new type.
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <ctime>

const double DA_RATE = 0.42;

// ---------------- alternative: single class + switch ----------------
class StaffAll {
public:
    enum Type { PROFESSOR, LAB_ASSISTANT, GUEST };
private:
    std::string name_; int id_; Type type_;
    double basic_, research_;     // used by PROFESSOR / LAB_ASSISTANT only
    double hours_, rate_;         // used by GUEST only      -> unused fields in every object
public:
    StaffAll(Type t, const std::string& n, int id, double a, double b)
        : name_(n), id_(id), type_(t), basic_(0), research_(0), hours_(0), rate_(0)
    {
        if (t == GUEST) { hours_ = a; rate_ = b; } else { basic_ = a; research_ = b; }
    }
    double pay() const
    {
        switch (type_) {
            case PROFESSOR:     return basic_ + basic_ * DA_RATE + research_;
            case LAB_ASSISTANT: return basic_ + basic_ * DA_RATE;
            case GUEST:         return hours_ * rate_;
        }                                            // a new type = edit this switch,
        return 0;                                    // the enum, the constructor, every other switch
    }
};

// ---------------- hierarchy (compact copy of the repaired design) ----------------
class Staff {
    std::string name_; int id_;
public:
    Staff(const std::string& n, int id) : name_(n), id_(id) {}
    virtual ~Staff() = default;
    virtual double pay() const = 0;
};
class SalariedStaff : public Staff {
    double basic_;
public:
    SalariedStaff(const std::string& n, int id, double b) : Staff(n, id), basic_(b) {}
    double pay() const override { return basic_ + basic_ * DA_RATE; }
};
class Professor : public SalariedStaff {
    double research_;
public:
    Professor(const std::string& n, int id, double b, double r) : SalariedStaff(n, id, b), research_(r) {}
    double pay() const override { return SalariedStaff::pay() + research_; }
};
class LabAssistant : public SalariedStaff {
public:
    LabAssistant(const std::string& n, int id, double b) : SalariedStaff(n, id, b) {}
    double pay() const override { return SalariedStaff::pay(); }
};
class GuestFaculty : public Staff {
    double hours_, rate_;
public:
    GuestFaculty(const std::string& n, int id, double h, double r) : Staff(n, id), hours_(h), rate_(r) {}
    double pay() const override { return hours_ * rate_; }
};

volatile double sink = 0;

int main()
{
    std::cout << "sizeof(StaffAll)     = " << sizeof(StaffAll) << " bytes (every object, every type)\n";
    std::cout << "sizeof(Professor)    = " << sizeof(Professor) << '\n';
    std::cout << "sizeof(LabAssistant) = " << sizeof(LabAssistant) << '\n';
    std::cout << "sizeof(GuestFaculty) = " << sizeof(GuestFaculty) << " (includes one vtable pointer each)\n\n";

    const int N = 1000, ROUNDS = 20000;                    // 20,000,000 pay() calls each
    std::vector<StaffAll> flat;
    std::vector< std::unique_ptr<Staff> > poly;
    for (int i = 0; i < N; ++i) {
        std::string n = "S" + std::to_string(i);
        switch (i % 3) {
            case 0: flat.push_back(StaffAll(StaffAll::PROFESSOR, n, i, 50000 + i, 3000));
                    poly.push_back(std::unique_ptr<Staff>(new Professor(n, i, 50000 + i, 3000))); break;
            case 1: flat.push_back(StaffAll(StaffAll::LAB_ASSISTANT, n, i, 25000 + i, 0));
                    poly.push_back(std::unique_ptr<Staff>(new LabAssistant(n, i, 25000 + i))); break;
            default: flat.push_back(StaffAll(StaffAll::GUEST, n, i, 10 + i % 20, 1200));
                    poly.push_back(std::unique_ptr<Staff>(new GuestFaculty(n, i, 10 + i % 20, 1200))); break;
        }
    }

    double a = 0, b = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) a += flat[i].pay();
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) b += poly[i]->pay();
    clock_t t2 = clock();
    sink = a + b;

    std::cout << "switch version  : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "virtual version : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "same total?       " << (a == b ? "yes" : "NO") << "  (" << a << ")\n";
    return 0;
}
