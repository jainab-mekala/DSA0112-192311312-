// Task 5.1 - Four ways to reach a Cart object + the this pointer
//   (a) directly by name   (b) through a pointer with ->   (c) through a reference
//   (d) through a pointer created with new and released with delete
// 'this' is used (1) to return *this for chaining and (2) to separate a member from a
// parameter with the same name.
#include <iostream>
#include <iomanip>
#include <string>

class Cart {
    std::string owner;
    std::string items[10];
    double      prices[10];
    int         n = 0;
    double      discount = 0;                 // percent
public:
    explicit Cart(const std::string& owner) : owner(owner) {}     // member 'owner' vs parameter 'owner'

    Cart& add(const std::string& item, double price)
    {
        if (n < 10) { items[n] = item; prices[n] = price; ++n; }
        return *this;                                              // *this is the object itself -> chaining
    }
    Cart& setDiscount(double discount)
    {
        this->discount = discount;                                 // this->discount = member; discount = parameter
        return *this;
    }
    Cart& setOwner(const std::string& owner)
    {
        this->owner = owner;
        return *this;
    }
    double total() const
    {
        double t = 0;
        for (int i = 0; i < n; ++i) t += prices[i];
        return t * (100 - discount) / 100;
    }
    const std::string& getOwner() const { return owner; }
    const Cart* address() const { return this; }                   // 'this' is a POINTER to the current object
};

void addMilk(Cart& c) { c.add("Milk", 30); }                       // (c) through a reference: no copy, original changes

int main()
{
    std::cout << std::fixed << std::setprecision(2);

    // (a) directly by name
    Cart a("Asha");
    a.add("Rice", 60).add("Dal", 120).setDiscount(10);             // chained calls through this
    std::cout << "(a) by name        : " << a.getOwner() << " total " << a.total() << '\n';

    // (b) through a pointer with ->
    Cart b("Bala");
    Cart* pb = &b;
    pb->add("Oil", 180).add("Salt", 20);
    std::cout << "(b) by pointer ->  : " << pb->getOwner() << " total " << pb->total()
              << "  (same object? " << (pb == b.address() ? "yes" : "no") << ")\n";

    // (c) through a reference
    Cart c("Chitra");
    Cart& rc = c;
    rc.add("Tea", 90);
    addMilk(c);                                                     // function takes Cart&
    std::cout << "(c) by reference   : " << rc.getOwner() << " total " << rc.total() << '\n';

    // (d) pointer created with new, released with delete
    Cart* pd = new Cart("Dev");
    pd->add("Sugar", 45).setDiscount(5);
    std::cout << "(d) new / delete   : " << pd->getOwner() << " total " << pd->total() << '\n';
    delete pd;                                                      // forgetting this leaks; using pd afterwards is UB
    pd = nullptr;

    // 'this' separating member from parameter with the same name
    a.setOwner("Asha K.");
    std::cout << "setOwner() used this->owner = owner: " << a.getOwner() << '\n';

    std::cout << "address of a: " << &a << "   a.address() returns: " << a.address()
              << "   (this == &a)\n";
    return 0;
}
