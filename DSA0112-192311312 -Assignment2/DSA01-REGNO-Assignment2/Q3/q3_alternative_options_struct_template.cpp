// Q3 - Alternative concept: ONE options struct (and a function template) instead of
// several overloads with default arguments.
#include <iostream>

struct Parcel {
    double kg;
    bool   fragile;
    explicit Parcel(double w, bool f = false) : kg(w), fragile(f) {}
};

// all optional features live in one struct: adding a new option = adding one field
struct Options {
    bool express;
    bool insured;
    bool fragile;
    bool weekend;        // <-- the "new option": only this line + one line in price() changed
    Options() : express(false), insured(false), fragile(false), weekend(false) {}
    Options& setExpress()  { express = true; return *this; }
    Options& setInsured()  { insured = true; return *this; }
    Options& setFragile()  { fragile = true; return *this; }
    Options& setWeekend()  { weekend = true; return *this; }
};

double price(double kg, const Options& o)
{
    double c = 40 + 30 * kg;
    if (o.express) c *= 1.5;
    if (o.insured) c += 25;
    if (o.fragile) c += 15;
    if (o.weekend) c += 10;
    return c;
}

// converting different inputs to kg: small overloaded helpers (resolved at compile time)
double toKg(int grams)           { return grams / 1000.0; }
double toKg(double kg)           { return kg; }
double toKg(const Parcel& p)     { return p.kg; }

// ONE function template handles all weight kinds
template <typename W>
double charge(const W& weight, const Options& o = Options())
{
    return price(toKg(weight), o);
}

int main()
{
    std::cout << "charge(2500)                      = " << charge(2500) << '\n';
    std::cout << "charge(2.5)                       = " << charge(2.5) << '\n';
    std::cout << "charge(2.5, express)              = " << charge(2.5, Options().setExpress()) << '\n';
    std::cout << "charge(Parcel(1.2), exp+ins)      = "
              << charge(Parcel(1.2), Options().setExpress().setInsured()) << '\n';
    std::cout << "charge(2.5, weekend) [new option] = " << charge(2.5, Options().setWeekend()) << '\n';
    std::cout << "\nNo run-time overload resolution exists: templates and overloads are both\n"
                 "chosen at compile time. Space copied per call: W by const reference + one Options (3-4 bools).\n";
    return 0;
}
