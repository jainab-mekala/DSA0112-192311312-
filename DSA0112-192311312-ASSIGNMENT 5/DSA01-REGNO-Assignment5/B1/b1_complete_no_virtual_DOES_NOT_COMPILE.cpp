// B1 - "Remove virtual from total() and explain the compiler error."
// THIS FILE IS MEANT TO FAIL TO COMPILE. Screenshot the error and explain it:
// '= 0' (pure specifier) is only allowed on a VIRTUAL function, so the declaration
//     double total() const = 0;
// is rejected ("initializer specified for non-virtual function"). A pure virtual function is a
// request for derived classes to supply the function that run-time binding will select;
// without 'virtual' there is no run-time binding, so there is nothing to make pure.
#include <iostream>
#include <string>

class Bill {
protected:
    std::string patient;
public:
    explicit Bill(const std::string& p) : patient(p) {}
    virtual ~Bill() = default;
    double total() const = 0;                       // <-- 'virtual' removed: ERROR
    virtual std::string type() const { return "Bill"; }
};

class OutPatient : public Bill {
    double consultation, tests;
public:
    OutPatient(const std::string& p, double c, double t) : Bill(p), consultation(c), tests(t) {}
    double total() const override { return consultation + tests; }
    std::string type() const override { return "OP"; }
};

int main()
{
    Bill* b = new OutPatient("Ravi", 500, 1200);
    std::cout << b->type() << ' ' << b->total() << '\n';
    delete b;
}
