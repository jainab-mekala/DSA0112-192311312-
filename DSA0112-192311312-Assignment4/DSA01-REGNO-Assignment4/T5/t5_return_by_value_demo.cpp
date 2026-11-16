// Task 5 - "Explain what would happen if add() returned Cart instead of Cart&."
// Here add() and setDiscount() return Cart BY VALUE. Every call in a chain works on a
// TEMPORARY COPY of the previous result, so only the first call changes 'c'.
// PREDICT the total printed for c before running.
#include <iomanip>
#include <iostream>
#include <string>

class Cart {
    std::string items[10];
    double prices[10];
    int n = 0;
    double discount = 0;
public:
    Cart add(const std::string& item, double price)           // <-- Cart, not Cart&
    {
        if (n < 10) { items[n] = item; prices[n] = price; ++n; }
        return *this;                                         // returns a COPY of *this
    }
    Cart setDiscount(double discount)                         // <-- Cart, not Cart&
    {
        this->discount = discount;
        return *this;
    }
    double total() const
    {
        double t = 0;
        for (int i = 0; i < n; ++i) t += prices[i];
        return t * (100 - discount) / 100;
    }
};

int main()
{
    Cart c;
    c.add("Rice", 60).add("Dal", 120).setDiscount(10);        // 1st add changes c; the rest change temporaries
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "total of c after the chain : " << c.total() << '\n';
    Cart* p = &c;
    p->add("Oil", 180);
    std::cout << "total of c after Oil       : " << p->total() << '\n';
    std::cout << "(with Cart& the totals would be 324.00 for the whole chain, see t5_complete_code.cpp)\n";
    return 0;
}
