// Task 5 - Alternative concept: std::vector<Cart> (objects) instead of an array of
// pointers to Cart. Compares copying, space, time to find the highest total (O(n) in
// both) and what happens to pointers into the vector when it grows.
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>

class Cart {
    std::string owner;
    double      prices[10];
    int         n = 0;
    double      discount = 0;
public:
    static int copies;                                       // counts copy constructions
    explicit Cart(const std::string& owner) : owner(owner) {}
    Cart(const Cart& o) : owner(o.owner), n(o.n), discount(o.discount)
    {
        for (int i = 0; i < n; ++i) prices[i] = o.prices[i];
        ++copies;
    }
    Cart& operator=(const Cart&) = default;
    Cart& add(double price) { if (n < 10) prices[n++] = price; return *this; }
    double total() const
    {
        double t = 0;
        for (int i = 0; i < n; ++i) t += prices[i];
        return t * (100 - discount) / 100;
    }
    const std::string& getOwner() const { return owner; }
};
int Cart::copies = 0;

volatile double sink = 0;

int main()
{
    std::cout << "sizeof(Cart) = " << sizeof(Cart) << " bytes; sizeof(Cart*) = " << sizeof(Cart*) << " bytes\n\n";

    // ---- growth: copies and invalidated pointers ----
    std::vector<Cart> v;
    v.push_back(Cart("Asha"));  v[0].add(100);
    Cart* pFirst = &v[0];                                   // pointer into the vector
    Cart::copies = 0;
    std::size_t cap = v.capacity();
    int pushed = 0;
    while (v.capacity() == cap) { v.push_back(Cart("X")); ++pushed; }
    std::cout << "pushed " << pushed << " more carts until the vector reallocated\n";
    std::cout << "Cart copy constructions during that growth (push + moving old objects): " << Cart::copies << '\n';
    std::cout << "pointer taken before growth still equals &v[0]? "
              << (pFirst == &v[0] ? "yes" : "NO -> it is dangling (do not use it)") << "\n\n";

    // ---- time to find the highest total ----
    const int N = 100000, ROUNDS = 100;
    std::vector<Cart> objs;
    std::vector<Cart*> ptrs;
    objs.reserve(N);
    for (int i = 0; i < N; ++i) objs.push_back(Cart("c"));
    for (int i = 0; i < N; ++i) { objs[i].add(i % 500).add(10); ptrs.push_back(&objs[i]); }   // pointers into a RESERVED vector (safe)

    double bestA = 0, bestB = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) {
        double best = -1;
        for (int i = 0; i < N; ++i) { double t = objs[i].total(); if (t > best) best = t; }
        bestA = best;
    }
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) {
        double best = -1;
        for (int i = 0; i < N; ++i) { double t = ptrs[i]->total(); if (t > best) best = t; }
        bestB = best;
    }
    clock_t t2 = clock();
    sink = bestA + bestB;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "vector<Cart>  : highest " << bestA << " in " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "array of Cart*: highest " << bestB << " in " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    return 0;
}
