// Q4 - Alternative: the same account, but the array is used as a CIRCULAR buffer.
// Adding a transaction is O(1): overwrite the oldest slot and move the start index.
#include <iostream>
#include <iomanip>

struct Transaction {
    char   type;
    double amount;
    double balanceAfter;
};

class SavingsAccount {
    enum { MAX_TXN = 10 };                 // change to 50: nothing else needs to change
    long        accountNo;
    double      balance;
    Transaction txn[MAX_TXN];
    int         start;                     // index of the OLDEST transaction
    int         count;

    void record(char type, double amount)
    {
        Transaction t;
        t.type = type; t.amount = amount; t.balanceAfter = balance;
        if (count < MAX_TXN) {
            txn[(start + count) % MAX_TXN] = t;
            ++count;
        } else {
            txn[start] = t;                // overwrite the oldest
            start = (start + 1) % MAX_TXN; // oldest moves forward - no shifting
        }
    }

public:
    SavingsAccount(long no, double opening) : accountNo(no), balance(opening), start(0), count(0) {}

    bool deposit(double amount)
    {
        if (amount <= 0) return false;
        balance += amount;
        record('D', amount);
        return true;
    }
    bool withdraw(double amount)
    {
        if (amount <= 0 || amount > balance) return false;
        balance -= amount;
        record('W', amount);
        return true;
    }
    void miniStatement() const
    {
        std::cout << "---- Mini statement (circular)  A/C " << accountNo << " ----\n";
        std::cout << std::fixed << std::setprecision(2);
        for (int i = 0; i < count; ++i) {
            const Transaction& t = txn[(start + i) % MAX_TXN];
            std::cout << std::setw(2) << i + 1 << ". "
                      << (t.type == 'D' ? "Deposit   " : "Withdrawal")
                      << std::setw(10) << t.amount
                      << "   balance " << std::setw(10) << t.balanceAfter << '\n';
        }
        std::cout << "Closing balance: " << balance << "\n\n";
    }
};

int main()
{
    SavingsAccount acc(1001, 5000.0);
    for (int i = 1; i <= 7; ++i) { acc.deposit(100.0 * i); acc.withdraw(50.0 * i); }
    std::cout << "After 14 transactions (only the last 10 are kept):\n";
    acc.miniStatement();
    return 0;
}
