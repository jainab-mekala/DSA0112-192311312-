// Q3 - Courier Charge Calculator: redesigned with default arguments + overloading
// Weight in grams (int), weight in kg (double), or a Parcel object.
// Defaults only at the right-hand side, and only ONE default-carrying function per
// weight type, so no call is ambiguous.
//
// BEFORE running: predict which overload each of the six calls in main() selects.
#include <iostream>

struct Parcel {
    double kg;
    bool   fragile;
    explicit Parcel(double w, bool f = false) : kg(w), fragile(f) {}   // explicit: no silent conversion
};

// shared pricing rule (not overloaded, so it never competes in resolution)
double price(double kg, bool express, bool insured, bool fragile = false)
{
    double c = 40 + 30 * kg;
    if (express) c *= 1.5;
    if (insured) c += 25;
    if (fragile) c += 15;
    return c;
}

// (1) weight in grams
double charge(int grams, bool express = false, bool insured = false)
{
    std::cout << "  [charge(int grams, ...)]       ";
    return price(grams / 1000.0, express, insured);
}

// (2) weight in kilograms
double charge(double kg, bool express = false, bool insured = false)
{
    std::cout << "  [charge(double kg, ...)]       ";
    return price(kg, express, insured);
}

// (3) a Parcel object
double charge(const Parcel& p, bool express = false, bool insured = false)
{
    std::cout << "  [charge(const Parcel&, ...)]   ";
    return price(p.kg, express, insured, p.fragile);
}

int main()
{
    std::cout << "call 1: charge(2500)\n";                     std::cout << charge(2500) << '\n';
    std::cout << "call 2: charge(2.5)\n";                      std::cout << charge(2.5) << '\n';
    std::cout << "call 3: charge(2.5, true)\n";                std::cout << charge(2.5, true) << '\n';
    std::cout << "call 4: charge(Parcel(1.2, true), true, true)\n";
    std::cout << charge(Parcel(1.2, true), true, true) << '\n';
    std::cout << "call 5: charge(2)\n";                        std::cout << charge(2) << '\n';
    std::cout << "call 6: charge(2.0f)\n";                     std::cout << charge(2.0f) << '\n';

    // Still ambiguous (try it!): long -> int and long -> double are BOTH conversions
    // std::cout << charge(2L) << '\n';          // <-- uncomment to see the ambiguity error
    // Fix: charge(static_cast<int>(2L));
    return 0;
}
