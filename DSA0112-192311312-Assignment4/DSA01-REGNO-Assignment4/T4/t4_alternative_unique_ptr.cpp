// Task 4 - Alternative concept: std::vector<std::unique_ptr<Account>> instead of raw pointers.
// Part 1 (namespace bad) : unique_ptr + the ORIGINAL classes -> which defects remain?
// Part 2 (namespace good): unique_ptr + virtual functions/destructor -> all fixed.
// Part 3: time for n accounts is O(n) in both; unique_ptr adds no space (same size as a raw pointer).
#include <iostream>
#include <memory>
#include <vector>
#include <ctime>

int liveHistory = 0;

namespace bad {                                   // original design, only the ownership changed
struct Account {
    double rate() const { return 3.5; }           // still not virtual
    ~Account() {}                                 // still not virtual
};
struct SeniorAccount : Account {
    double* history = new double[100];
    SeniorAccount() { ++liveHistory; }
    double rate() const { return 4.0; }
    ~SeniorAccount() { delete[] history; --liveHistory; }
};
}

namespace good {
struct Account {
    virtual ~Account() = default;
    virtual double rate() const { return 3.5; }
};
struct SeniorAccount : Account {
    double* history = new double[100];
    SeniorAccount() { ++liveHistory; }
    ~SeniorAccount() override { delete[] history; --liveHistory; }
    double rate() const override { return 4.0; }
};
}

volatile double sink = 0;

int main()
{
    {
        std::vector< std::unique_ptr<bad::Account> > v;
        v.push_back(std::unique_ptr<bad::Account>(new bad::Account));
        v.push_back(std::unique_ptr<bad::Account>(new bad::SeniorAccount));
        std::cout << "unique_ptr + original classes:\n";
        for (auto& a : v) std::cout << "  rate " << a->rate() << '\n';          // defect 1 remains
        v.clear();                                                              // defect 2 remains
        std::cout << "  history arrays alive after clear(): " << liveHistory << "   (leak remains)\n";
        bad::SeniorAccount sen;
        bad::Account sliced = sen;                                              // defect 3 (slicing) remains
        std::cout << "  sliced copy rate: " << sliced.rate() << "   (slicing is still possible)\n\n";
    }
    liveHistory = 0;                                                            // reset the counter for part 2

    {
        std::vector< std::unique_ptr<good::Account> > v;
        v.push_back(std::unique_ptr<good::Account>(new good::Account));
        v.push_back(std::unique_ptr<good::Account>(new good::SeniorAccount));
        std::cout << "unique_ptr + virtual design:\n";
        for (auto& a : v) std::cout << "  rate " << a->rate() << '\n';
        v.clear();
        std::cout << "  history arrays alive after clear(): " << liveHistory << "   (no leak)\n\n";
    }

    std::cout << "sizeof(good::Account*) = " << sizeof(good::Account*)
              << ", sizeof(unique_ptr<good::Account>) = " << sizeof(std::unique_ptr<good::Account>) << '\n';

    const int N = 1000000;
    std::vector<good::Account*> raw;
    std::vector< std::unique_ptr<good::Account> > smart;
    for (int i = 0; i < N; ++i) { raw.push_back(new good::Account); smart.push_back(std::unique_ptr<good::Account>(new good::Account)); }
    double a = 0, b = 0;
    clock_t t0 = clock();
    for (int r = 0; r < 20; ++r) for (int i = 0; i < N; ++i) a += raw[i]->rate();
    clock_t t1 = clock();
    for (int r = 0; r < 20; ++r) for (int i = 0; i < N; ++i) b += smart[i]->rate();
    clock_t t2 = clock();
    sink = a + b;
    for (auto p : raw) delete p;
    std::cout << "raw pointers : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "unique_ptr   : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    return 0;
}
