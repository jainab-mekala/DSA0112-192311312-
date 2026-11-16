// Task 5.2 - An array of pointers to Cart objects for all active customers.
// Finds the cart with the highest total using (1) indexing and (2) pointer arithmetic.
#include <iostream>
#include <iomanip>
#include <string>

class Cart {
    std::string owner;
    double      prices[10];
    int         n = 0;
    double      discount = 0;
public:
    explicit Cart(const std::string& owner) : owner(owner) {}
    Cart& add(double price) { if (n < 10) prices[n++] = price; return *this; }
    Cart& setDiscount(double discount) { this->discount = discount; return *this; }
    double total() const
    {
        double t = 0;
        for (int i = 0; i < n; ++i) t += prices[i];
        return t * (100 - discount) / 100;
    }
    const std::string& getOwner() const { return owner; }
};

// (1) indexing
Cart* highestByIndex(Cart* carts[], int count)
{
    Cart* best = carts[0];
    for (int i = 1; i < count; ++i)
        if (carts[i]->total() > best->total()) best = carts[i];
    return best;
}

// (2) pointer arithmetic: 'p' walks over the array of pointers
Cart* highestByPointer(Cart** first, int count)
{
    Cart* best = *first;
    for (Cart** p = first + 1; p < first + count; ++p)       // p++ moves to the next Cart*
        if ((*p)->total() > best->total()) best = *p;
    return best;
}

int main()
{
    Cart* active[5];                                         // array of pointers to Cart objects
    active[0] = new Cart("Asha");   active[0]->add(60).add(120).setDiscount(10);
    active[1] = new Cart("Bala");   active[1]->add(180).add(20);
    active[2] = new Cart("Chitra"); active[2]->add(90).add(30).add(250);
    active[3] = new Cart("Dev");    active[3]->add(45).setDiscount(5);
    active[4] = new Cart("Esha");   active[4]->add(15).add(15);

    std::cout << std::fixed << std::setprecision(2);
    for (int i = 0; i < 5; ++i)
        std::cout << std::setw(8) << active[i]->getOwner() << " : " << active[i]->total() << '\n';

    Cart* a = highestByIndex(active, 5);
    Cart* b = highestByPointer(active, 5);
    std::cout << "\nhighest (indexing)           : " << a->getOwner() << " " << a->total() << '\n';
    std::cout << "highest (pointer arithmetic) : " << b->getOwner() << " " << b->total() << '\n';
    std::cout << "same cart found by both?       " << (a == b ? "yes" : "no") << '\n';

    for (int i = 0; i < 5; ++i) delete active[i];            // the array owns the objects
    return 0;
}
