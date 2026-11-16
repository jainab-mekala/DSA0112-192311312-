// Task 4.3 - Repaired program: virtual function + override, virtual destructor,
// pointers/references instead of copies, copying of polymorphic objects forbidden.
//
// Task 4.4 - Prevention rules (write in your report):
//   Rule 1: a class with virtual functions (any class meant to be a base) must have a VIRTUAL destructor.
//   Rule 2: mark every overriding function 'override' so a mismatch is a compile error, and
//           never copy polymorphic objects by value - hold them by pointer / reference / smart pointer.
#include <iostream>

int liveHistoryArrays = 0;

class Account {
public:
    Account() = default;
    Account(const Account&) = delete;                 // forbids slicing copies:  Account c = senior;  -> compile error
    Account& operator=(const Account&) = delete;
    virtual ~Account() { std::cout << "  ~Account\n"; }
    virtual double rate() const { return 3.5; }
};

class SeniorAccount : public Account {
    double* history = new double[100];
public:
    SeniorAccount() { ++liveHistoryArrays; std::cout << "  SeniorAccount created (history array allocated)\n"; }
    ~SeniorAccount() override { delete[] history; --liveHistoryArrays; std::cout << "  ~SeniorAccount (history released)\n"; }
    double rate() const override { return 4.0; }
};

void printRate(const Account& a) { std::cout << "  " << a.rate() << "\n"; }    // by reference: no copy, no slicing

int main()
{
    Account* accounts[2] = { new Account, new SeniorAccount };

    std::cout << "rates through Account*:\n";
    for (Account* a : accounts) std::cout << "  " << a->rate() << "\n";

    std::cout << "rates through const Account&:\n";
    for (Account* a : accounts) printRate(*a);

    std::cout << "deleting through Account*:\n";
    for (Account* a : accounts) delete a;
    std::cout << "history arrays still alive after delete: " << liveHistoryArrays << "   (0 = no leak)\n\n";

    SeniorAccount s;
    const Account& ref = s;                            // reference to the SAME object instead of a sliced copy
    std::cout << "through a reference: " << ref.rate() << '\n';
    // Account copy = s;                               // ERROR now: use of deleted function (slicing prevented)
    return 0;
}
