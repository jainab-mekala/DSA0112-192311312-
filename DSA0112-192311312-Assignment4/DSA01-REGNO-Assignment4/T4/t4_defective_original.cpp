// Task 4.1 / 4.2 - The DEFECTIVE bank program (logic exactly as given) with evidence added.
//
// Defects to explain:
//  1. rate() is not virtual   -> a->rate() is bound at COMPILE time to Account::rate(): 3.5 for everyone
//  2. ~Account() is not virtual -> 'delete a' on an Account* runs only ~Account(); the
//     SeniorAccount destructor never runs, so history (new double[100]) leaks. (Formally UB.)
//  3. 'Account copy = SeniorAccount();' SLICES: only the Account part is copied; rate() is 3.5.
//
// Evidence:  (a) the messages printed by the destructors and the live-array counter below
//            (b) on Linux / WSL / MSYS2:  g++ -std=c++17 -g -fsanitize=address -Wall -Wextra t4_defective_original.cpp
//                AddressSanitizer reports the problem (new-delete-type-mismatch and/or leaked memory).
//            NOTE: MinGW/TDM-GCC that ships with old Dev-C++ does NOT support -fsanitize=address.
#include <iostream>

int liveHistoryArrays = 0;                 // instrumentation: how many history arrays exist

class Account {
public:
    double rate() const { return 3.5; }
    ~Account() { std::cout << "  ~Account\n"; }
};

class SeniorAccount : public Account {
    double* history = new double[100];
public:
    SeniorAccount() { ++liveHistoryArrays; std::cout << "  SeniorAccount created (history array allocated)\n"; }
    double rate() const { return 4.0; }
    ~SeniorAccount() { delete[] history; --liveHistoryArrays; std::cout << "  ~SeniorAccount (history released)\n"; }
};

int main()
{
    Account* accounts[2] = { new Account, new SeniorAccount };

    std::cout << "rates through Account*:\n";
    for (Account* a : accounts) std::cout << "  " << a->rate() << "\n";      // defect 1

    std::cout << "deleting through Account*:\n";
    for (Account* a : accounts) delete a;                                    // defect 2
    std::cout << "history arrays still alive after delete: " << liveHistoryArrays
              << "   <-- leak (expected 0)\n\n";

    std::cout << "slicing:\n";
    Account copy = SeniorAccount();                                          // defect 3
    std::cout << "  copy.rate() = " << copy.rate() << "   <-- the Senior part was sliced away\n";
    return 0;
}
