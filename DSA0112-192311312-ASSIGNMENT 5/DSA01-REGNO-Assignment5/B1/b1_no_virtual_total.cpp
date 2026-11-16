// B1.3 - Prove runtime binding: the same bills processed through base-class pointers with
// (namespace withVirtual) and without (namespace noVirtual) the keyword 'virtual' on total().
// PREDICT both lines of output before running.
#include <iostream>

namespace withVirtual {
struct Bill { virtual ~Bill() = default; virtual double total() const { return 0; } };
struct OutPatientBill : Bill { double total() const override { return 1700; } };
struct InPatientBill  : Bill { double total() const override { return 52500; } };
}

namespace noVirtual {
struct Bill { virtual ~Bill() = default;                 double total() const { return 0; } };   // 'virtual' removed
struct OutPatientBill : Bill { double total() const { return 1700; } };                          // hides, does not override
struct InPatientBill  : Bill { double total() const { return 52500; } };
}

template <typename B, typename OP, typename IP>
double dayTotal()
{
    B* bills[2] = { new OP, new IP };
    double sum = 0;
    for (int i = 0; i < 2; ++i) sum += bills[i]->total();    // call through a BASE-CLASS pointer
    for (int i = 0; i < 2; ++i) delete bills[i];
    return sum;
}

int main()
{
    std::cout << "with virtual total()    : day total = "
              << dayTotal<withVirtual::Bill, withVirtual::OutPatientBill, withVirtual::InPatientBill>()
              << "   (expected 54200)\n";
    std::cout << "without virtual total() : day total = "
              << dayTotal<noVirtual::Bill, noVirtual::OutPatientBill, noVirtual::InPatientBill>()
              << "   (the base-class total() is called for every bill: early binding)\n";
    return 0;
}
