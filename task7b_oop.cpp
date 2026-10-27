// Task 7 (Version B) - OBJECT-ORIENTED smart-card program
// The balance is private; only recharge() and deduct() can change it.
// Compatible with Dev-C++ (C++11).

#include <iostream>
using namespace std;

const long long MIN_BALANCE_PAISE = 5000;

class SmartCard
{
private:
    long long balancePaise;                // hidden from outside code
public:
    SmartCard(long long opening) : balancePaise(opening) {}

    long long getBalance() const { return balancePaise; }

    bool canTravel(long long farePaise) const
    {
        return balancePaise - farePaise >= MIN_BALANCE_PAISE;
    }

    bool deduct(long long farePaise)
    {
        if (farePaise < 0 || !canTravel(farePaise))
            return false;
        balancePaise -= farePaise;
        return true;
    }

    bool recharge(long long paise)
    {
        if (paise <= 0)
            return false;
        balancePaise += paise;
        return true;
    }
};

int main()
{
    SmartCard card(10000);                 // Rs 100

    cout << "Pay 2000: " << (card.deduct(2000) ? "OK" : "REFUSED")
         << ", balance = " << card.getBalance() << endl;
    cout << "Pay 4000: " << (card.deduct(4000) ? "OK" : "REFUSED")
         << ", balance = " << card.getBalance() << endl;
    cout << "Recharge -500: " << (card.recharge(-500) ? "OK" : "REFUSED")
         << ", balance = " << card.getBalance() << endl;
    cout << "Recharge 1000: " << (card.recharge(1000) ? "OK" : "REFUSED")
         << ", balance = " << card.getBalance() << endl;

    // card.balancePaise = -999;           // ERROR: 'balancePaise' is private
    cout << "The line 'card.balancePaise = -999;' does not compile, so the card is protected." << endl;
    return 0;
}
