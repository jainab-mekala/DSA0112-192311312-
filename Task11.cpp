#include <iostream>
#include <iomanip>
using namespace std;

enum class Zone { Inner = 1, Middle, Outer };
constexpr long long MIN_BALANCE_PAISE = 5000;
constexpr int PEAK_PERCENT = 20;

class FareTable {
public:
    long long baseFare(Zone z) const {
        switch (z) {
            case Zone::Inner: return 1000;
            case Zone::Middle: return 2000;
            case Zone::Outer: return 3000;
        }
        return -1;
    }
    long long fare(Zone z, int hour) const {
        long long f = baseFare(z);
        if (f < 0 || hour < 0 || hour > 23) return -1;
        bool peak = (hour >= 8 && hour < 10) || (hour >= 17 && hour < 20);
        if (peak) f += f * PEAK_PERCENT / 100;
        return f;
    }
};
class SmartCard {
    long long balancePaise;
public:
    explicit SmartCard(long long balance) : balancePaise(balance) {}
    bool deduct(long long fare) {
        if (fare < 0 || balancePaise - fare < MIN_BALANCE_PAISE) return false;
        balancePaise -= fare;
        return true;
    }
    long long balance() const { return balancePaise; }
};
int main() {
    FareTable fares;
    SmartCard card(10000);
    long long f = fares.fare(Zone::Outer, 18);
    cout << "Recommended design: enum class, constexpr rules, integer paise, private card balance.\n";
    cout << "Outer-zone peak fare: Rs. " << fixed << setprecision(2) << f / 100.0 << '\n';
    cout << "Journey accepted? " << (card.deduct(f) ? "Yes" : "No") << '\n';
    cout << "Remaining balance: Rs. " << card.balance() / 100.0 << '\n';
    cout << "Complexity: fare calculation O(1); balance storage O(1) per card.\n";
    cout << "Limitation: changing a compiled fare requires rebuilding; a config file can allow runtime updates.\n";
    return 0;
}
