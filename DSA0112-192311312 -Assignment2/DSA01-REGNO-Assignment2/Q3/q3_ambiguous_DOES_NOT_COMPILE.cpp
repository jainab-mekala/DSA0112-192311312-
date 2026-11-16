// Q3 - The developer's ORIGINAL code. THIS FILE IS MEANT TO FAIL TO COMPILE.
// Compile it, copy / screenshot the "call of overloaded 'charge(...)' is ambiguous"
// errors as evidence, and explain them in your report.
#include <iostream>

double charge(double weight)
{
    return 40 + 30 * weight;
}
double charge(double weight, bool express)
{
    return charge(weight) * (express ? 1.5 : 1.0);
}
double charge(double weight, bool express = false, bool insured = false)
{
    double c = charge(weight, express);
    return insured ? c + 25 : c;
}

int main()
{
    std::cout << charge(2.5) << '\n';          // candidates: (double) and (double,bool=false,bool=false)
    std::cout << charge(2.5, true) << '\n';    // candidates: (double,bool) and (double,bool,bool=false)
    return 0;
}
