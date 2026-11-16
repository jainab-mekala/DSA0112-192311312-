// Q7 - Diagnose, Redesign and Validate  (worked example based on Q4)
//
// ORIGINAL : SavingsAccount keeps the last K transactions in a plain array and
//            SHIFTS every element left when the array is full  -> O(K) per transaction.
// REDESIGN : same public interface, but the array is used as a CIRCULAR BUFFER with a
//            start index                                       -> O(1) per transaction.
//
// The program:
//   1. validates that both versions give IDENTICAL statements (correctness test)
//   2. measures both versions with the SAME workload for K = 10, 50, 100, 500
//   3. writes the table to q7_results.csv so you can attach it to your report
//
// You may choose a different program from Q1-Q6 as your Q7 subject; if you do, follow the
// same structure (diagnose -> objective -> redesign -> predict -> validate -> defend).
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>

struct Transaction { char type; double amount; double balanceAfter; };

// ------------------------- ORIGINAL (shifting array) -------------------------
template <int K>
class AccountShift {
    double balance; Transaction txn[K]; int count;
    void record(char type, double amount)
    {
        Transaction t; t.type = type; t.amount = amount; t.balanceAfter = balance;
        if (count < K) { txn[count++] = t; return; }
        for (int i = 0; i < K - 1; ++i) txn[i] = txn[i + 1];      // O(K)
        txn[K - 1] = t;
    }
public:
    AccountShift() : balance(0), count(0) {}
    bool deposit(double a)  { if (a <= 0) return false; balance += a; record('D', a); return true; }
    bool withdraw(double a) { if (a <= 0 || a > balance) return false; balance -= a; record('W', a); return true; }
    std::string statement() const
    {
        std::ostringstream os;
        for (int i = 0; i < count; ++i)
            os << txn[i].type << ' ' << txn[i].amount << ' ' << txn[i].balanceAfter << '\n';
        return os.str();
    }
    double getBalance() const { return balance; }
};

// ------------------------- REDESIGN (circular buffer) ------------------------
template <int K>
class AccountCircular {
    double balance; Transaction txn[K]; int start, count;
    void record(char type, double amount)
    {
        Transaction t; t.type = type; t.amount = amount; t.balanceAfter = balance;
        if (count < K) { txn[(start + count) % K] = t; ++count; return; }
        txn[start] = t; start = (start + 1) % K;                  // O(1)
    }
public:
    AccountCircular() : balance(0), start(0), count(0) {}
    bool deposit(double a)  { if (a <= 0) return false; balance += a; record('D', a); return true; }
    bool withdraw(double a) { if (a <= 0 || a > balance) return false; balance -= a; record('W', a); return true; }
    std::string statement() const
    {
        std::ostringstream os;
        for (int i = 0; i < count; ++i) {
            const Transaction& t = txn[(start + i) % K];
            os << t.type << ' ' << t.amount << ' ' << t.balanceAfter << '\n';
        }
        return os.str();
    }
    double getBalance() const { return balance; }
};

// ------------------------- the same workload for both -----------------------
volatile double sink = 0;

template <typename Account>
double runWorkload(int ops, std::string* finalStatement)
{
    Account acc;
    clock_t t0 = clock();
    for (int i = 0; i < ops; ++i) {
        if (i % 2 == 0) acc.deposit(100.0 + (i % 7));
        else            acc.withdraw(50.0 + (i % 5));
    }
    clock_t t1 = clock();
    sink = acc.getBalance();
    if (finalStatement) *finalStatement = acc.statement();
    return 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
}

template <int K>
bool validate(int ops)
{
    std::string s1, s2;
    runWorkload< AccountShift<K> >(ops, &s1);
    runWorkload< AccountCircular<K> >(ops, &s2);
    return s1 == s2;
}

template <int K>
void measure(int ops, std::ofstream& csv)
{
    const int REPEATS = 3;
    double bestShift = 1e18, bestCirc = 1e18;
    for (int r = 0; r < REPEATS; ++r) {                   // best of 3 reduces timer noise
        double a = runWorkload< AccountShift<K> >(ops, 0);
        double b = runWorkload< AccountCircular<K> >(ops, 0);
        if (a < bestShift) bestShift = a;
        if (b < bestCirc)  bestCirc = b;
    }
    double nsShift = 1e6 * bestShift / ops, nsCirc = 1e6 * bestCirc / ops;
    std::cout << std::setw(5) << K << std::setw(14) << bestShift << std::setw(14) << bestCirc
              << std::setw(12) << nsShift << std::setw(12) << nsCirc
              << std::setw(10) << (bestCirc > 0 ? bestShift / bestCirc : 0.0) << "x\n";
    csv << K << ',' << bestShift << ',' << bestCirc << ',' << nsShift << ',' << nsCirc << '\n';
}

int main()
{
    const int OPS = 500000;

    std::cout << "Correctness (identical statements, 100000 operations):\n";
    std::cout << "  K=10  : " << (validate<10>(100000)  ? "PASS" : "FAIL") << '\n';
    std::cout << "  K=50  : " << (validate<50>(100000)  ? "PASS" : "FAIL") << '\n';
    std::cout << "  K=100 : " << (validate<100>(100000) ? "PASS" : "FAIL") << '\n';
    std::cout << "  K=500 : " << (validate<500>(100000) ? "PASS" : "FAIL") << "\n\n";

    std::cout << OPS << " transactions per run, best of 3\n";
    std::cout << std::setw(5) << "K" << std::setw(14) << "shift(ms)" << std::setw(14) << "circular(ms)"
              << std::setw(12) << "shift ns/op" << std::setw(12) << "circ ns/op"
              << std::setw(11) << "speed-up\n";

    std::ofstream csv("q7_results.csv");
    csv << "K,shift_ms,circular_ms,shift_ns_per_op,circular_ns_per_op\n";
    measure<10>(OPS, csv);
    measure<50>(OPS, csv);
    measure<100>(OPS, csv);
    measure<500>(OPS, csv);
    csv.close();

    std::cout << "\nResults also written to q7_results.csv\n";
    return 0;
}
