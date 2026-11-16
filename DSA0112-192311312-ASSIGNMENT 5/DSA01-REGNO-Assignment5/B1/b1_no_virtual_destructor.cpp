// B1.4 - Prove the virtual destructor. Derived bills own memory (a notes buffer).
// 1. Run with VIRTUAL_DTOR 1  -> clean.
// 2. Set VIRTUAL_DTOR 0, compile with  g++ -std=c++17 -g -fsanitize=address -Wall -Wextra b1_no_virtual_destructor.cpp
//    -> -Wall warns, AddressSanitizer reports new-delete-type-mismatch / leaked memory,
//       and the "~OutPatientBill" message disappears from the output.
#include <iostream>

#define VIRTUAL_DTOR 1

struct Bill {
#if VIRTUAL_DTOR
    virtual ~Bill() { std::cout << "~Bill\n"; }
#else
    ~Bill() { std::cout << "~Bill\n"; }
#endif
    virtual double total() const = 0;
};

struct OutPatientBill : Bill {
    char* notes = new char[1000];                       // owned resource
    ~OutPatientBill() { delete[] notes; std::cout << "~OutPatientBill (notes released)\n"; }
    double total() const override { return 1700; }
};

struct InPatientBill : Bill {
    double* charges = new double[200];                  // owned resource
    ~InPatientBill() { delete[] charges; std::cout << "~InPatientBill (charges released)\n"; }
    double total() const override { return 52500; }
};

int main()
{
    Bill* bills[2] = { new OutPatientBill, new InPatientBill };
    double sum = 0;
    for (int i = 0; i < 2; ++i) sum += bills[i]->total();
    std::cout << "day total " << sum << '\n';
    for (int i = 0; i < 2; ++i) delete bills[i];        // deleted through Bill*
    return 0;
}
