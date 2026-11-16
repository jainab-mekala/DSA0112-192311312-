// A2 - The same program with 'virtual' REMOVED from ~Bill().
// Predict the new output first. Then compile with:
//     g++ -std=c++17 -g -fsanitize=address -Wall -Wextra a2_no_virtual_destructor.cpp
// (-Wall also prints a warning: deleting an object with a non-virtual destructor.)
// NOTE: "override" had to be removed from ~OPBill() too: a destructor cannot be marked override
//       when the base destructor is not virtual (that is a compile error).
// NOTE: here OPBill adds no data, so sizeof(OPBill) == sizeof(Bill) and ASAN may stay silent
//       even though ~OPBill() never runs. See a2_no_virtual_destructor_with_member.cpp for
//       a version where the sanitizer reports the problem.
#include <iostream>

struct Bill {
    ~Bill() { std::cout << "~Bill "; }                 // 'virtual' removed
    virtual double total() const = 0;
    void show() const { std::cout << total() << ' '; }
};

struct OPBill : Bill {
    ~OPBill() { std::cout << "~OPBill "; }
    double total() const override { return 500; }
};

int main()
{
    Bill* b = new OPBill;
    b->show();
    delete b;
    std::cout << '\n';
}
