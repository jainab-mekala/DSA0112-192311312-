// A2 - Non-virtual destructor, where the derived class OWNS memory.
// Compile and run under the address sanitizer (Linux / WSL / MSYS2):
//     g++ -std=c++17 -g -fsanitize=address -Wall -Wextra a2_no_virtual_destructor_with_member.cpp
// Expected: ~OPBill() never runs, so 'rates' is never released -> AddressSanitizer reports
// new-delete-type-mismatch and / or a memory leak. Set VIRTUAL_DTOR to 1 to see a clean run.
#include <iostream>

#define VIRTUAL_DTOR 0

struct Bill {
#if VIRTUAL_DTOR
    virtual ~Bill() { std::cout << "~Bill "; }
#else
    ~Bill() { std::cout << "~Bill "; }
#endif
    virtual double total() const = 0;
    void show() const { std::cout << total() << ' '; }
};

struct OPBill : Bill {
    double* rates = new double[100];                   // resource owned by the derived class
    ~OPBill() { delete[] rates; std::cout << "~OPBill(rates released) "; }
    double total() const override { return 500; }
};

int main()
{
    Bill* b = new OPBill;
    b->show();
    delete b;
    std::cout << '\n';
}
