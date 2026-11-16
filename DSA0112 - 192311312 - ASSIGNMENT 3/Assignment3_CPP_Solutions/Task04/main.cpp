#include <iomanip>
#include <iostream>

class Money {
private:
    long long paise;

public:
    Money(long long rupees = 0, int p = 0)
        : paise(rupees * 100 + p) {}

    Money operator+(const Money& other) const {
        Money result;
        result.paise = paise + other.paise;
        return result;
    }

    Money operator-(const Money& other) const {
        Money result;
        result.paise = paise - other.paise;
        return result;
    }

    Money& operator+=(const Money& other) {
        paise += other.paise;
        return *this;
    }

    bool operator==(const Money& other) const {
        return paise == other.paise;
    }

    bool operator<(const Money& other) const {
        return paise < other.paise;
    }

    friend std::ostream& operator<<(std::ostream& out, const Money& money) {
        long long absolutePaise = money.paise < 0 ? -money.paise : money.paise;
        if (money.paise < 0) out << '-';
        out << "Rs " << absolutePaise / 100 << '.'
            << std::setw(2) << std::setfill('0') << absolutePaise % 100
            << std::setfill(' ');
        return out;
    }

    friend std::istream& operator>>(std::istream& in, Money& money) {
        long long rupees;
        int p;
        if (in >> rupees >> p) money.paise = rupees * 100 + p;
        return in;
    }
};

int main() {
    Money a(120, 75), b(79, 25);
    Money total = a + b;
    std::cout << total << '\n';
    std::cout << std::boolalpha << (total == Money(200)) << '\n';

    Money carry = Money(0, 75) + Money(0, 50);
    std::cout << "Paise carry: " << carry << '\n';

    Money balance = Money(10, 0);
    balance += Money(2, 50);
    std::cout << "Updated balance: " << balance << '\n';
    std::cout << "Subtraction: " << (Money(2, 0) - Money(3, 25)) << '\n';
    return 0;
}
