// Task 7 (Version A) - PROCEDURAL smart-card program
// Data (balance) and functions are separate. Compatible with Dev-C++ (C++11).

#include <iostream>
using namespace std;

const long long MIN_BALANCE_PAISE = 5000;

// The functions receive the balance as a reference; nothing protects it.
bool canTravel(long long balancePaise, long long farePaise)
{
    return balancePaise - farePaise >= MIN_BALANCE_PAISE;
}

bool deduct(long long &balancePaise, long long farePaise)
{
    if (!canTravel(balancePaise, farePaise))
        return false;
    balancePaise -= farePaise;
    return true;
}

int main()
{
    long long balancePaise = 10000;                    // Rs 100

    cout << "Pay 2000: " << (deduct(balancePaise, 2000) ? "OK" : "REFUSED")
         << ", balance = " << balancePaise << endl;
    cout << "Pay 4000: " << (deduct(balancePaise, 4000) ? "OK" : "REFUSED")
         << ", balance = " << balancePaise << endl;

    // PROBLEM: any code can change the balance directly and break the rules.
    balancePaise = -999;
    cout << "After careless line 'balancePaise = -999;' balance = " << balancePaise << endl;
    cout << "Nothing stopped this change." << endl;
    return 0;
}
