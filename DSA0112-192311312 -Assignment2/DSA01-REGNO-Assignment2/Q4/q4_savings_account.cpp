// Q4 - Savings Account Mini-Statement: class with an array inside it.
// Keeps the last MAX_TXN transactions by SHIFTING the array when it is full.
// Some member functions are defined inside the class, some outside using ::
//
// To keep the last 50 transactions change ONE number: MAX_TXN below.
#include <iostream>
#include <iomanip>

struct Transaction {
    char   type;           // 'D' = deposit, 'W' = withdrawal
    double amount;
    double balanceAfter;
};

class SavingsAccount {
    enum { MAX_TXN = 10 };                 // <-- change to 50 for "last 50 transactions"
    long        accountNo;                 // private: only member functions may touch the data
    double      balance;
    Transaction txn[MAX_TXN];              // array inside the class
    int         count;                     // how many slots are in use (0..MAX_TXN)

    void record(char type, double amount); // defined outside

public:
    // defined INSIDE the class (implicitly inline)
    SavingsAccount(long no, double opening) : accountNo(no), balance(opening), count(0) {}
    double getBalance() const { return balance; }
    long   getAccountNo() const { return accountNo; }

    bool deposit(double amount)
    {
        if (amount <= 0) return false;
        balance += amount;
        record('D', amount);
        return true;
    }

    // defined OUTSIDE the class with the scope resolution operator
    bool withdraw(double amount);
    void miniStatement() const;
};

void SavingsAccount::record(char type, double amount)
{
    Transaction t;
    t.type = type;
    t.amount = amount;
    t.balanceAfter = balance;

    if (count < MAX_TXN) {
        txn[count++] = t;                          // room left
    } else {                                       // 11th transaction: discard the oldest
        for (int i = 0; i < MAX_TXN - 1; ++i)      // shift left: O(MAX_TXN) work
            txn[i] = txn[i + 1];
        txn[MAX_TXN - 1] = t;
    }
}

bool SavingsAccount::withdraw(double amount)
{
    if (amount <= 0 || amount > balance) return false;
    balance -= amount;
    record('W', amount);
    return true;
}

void SavingsAccount::miniStatement() const
{
    std::cout << "---- Mini statement  A/C " << accountNo << " ----\n";
    std::cout << std::fixed << std::setprecision(2);
    for (int i = 0; i < count; ++i)                // oldest -> newest
        std::cout << std::setw(2) << i + 1 << ". "
                  << (txn[i].type == 'D' ? "Deposit   " : "Withdrawal")
                  << std::setw(10) << txn[i].amount
                  << "   balance " << std::setw(10) << txn[i].balanceAfter << '\n';
    std::cout << "Closing balance: " << balance << "\n\n";
}

int main()
{
    SavingsAccount acc(1001, 5000.0);

    // 8 transactions: still fits
    for (int i = 1; i <= 4; ++i) { acc.deposit(100.0 * i); acc.withdraw(50.0 * i); }
    std::cout << "After 8 transactions:\n";
    acc.miniStatement();

    // 14 transactions in total: the oldest 4 must have been discarded
    for (int i = 5; i <= 7; ++i) { acc.deposit(100.0 * i); acc.withdraw(50.0 * i); }
    std::cout << "After 14 transactions (only the last 10 are kept):\n";
    acc.miniStatement();

    std::cout << "invalid withdraw (too large) accepted? " << std::boolalpha << acc.withdraw(1e9) << '\n';
    return 0;
}
