// Q4 - Alternative concept analysis: time to add a transaction
//      shifting array  O(K) per add   vs   circular buffer O(1)   vs   std::deque O(1)
// where K = number of kept transactions. Run it at -O0 and -O2 and record the table.
#include <iostream>
#include <deque>
#include <ctime>

struct Transaction { char type; double amount; double balanceAfter; };

volatile double sink = 0;

template <int K>
struct ShiftLog {
    Transaction t[K]; int count;
    ShiftLog() : count(0) {}
    void add(const Transaction& x)
    {
        if (count < K) { t[count++] = x; return; }
        for (int i = 0; i < K - 1; ++i) t[i] = t[i + 1];     // O(K)
        t[K - 1] = x;
    }
    double last() const { return t[count - 1].amount; }
};

template <int K>
struct CircularLog {
    Transaction t[K]; int start, count;
    CircularLog() : start(0), count(0) {}
    void add(const Transaction& x)
    {
        if (count < K) { t[(start + count) % K] = x; ++count; return; }
        t[start] = x; start = (start + 1) % K;               // O(1)
    }
    double last() const { return t[(start + count - 1) % K].amount; }
};

template <int K>
struct DequeLog {
    std::deque<Transaction> d;
    void add(const Transaction& x)
    {
        d.push_back(x);                                      // O(1)
        if ((int)d.size() > K) d.pop_front();                // O(1)
    }
    double last() const { return d.back().amount; }
};

template <typename Log>
double run(int ops)
{
    Log log;
    Transaction x; x.type = 'D'; x.balanceAfter = 0;
    clock_t t0 = clock();
    for (int i = 0; i < ops; ++i) { x.amount = i; log.add(x); }
    clock_t t1 = clock();
    sink = log.last();
    return 1000.0 * (t1 - t0) / CLOCKS_PER_SEC;
}

template <int K>
void row(int ops)
{
    double a = run< ShiftLog<K> >(ops);
    double b = run< CircularLog<K> >(ops);
    double c = run< DequeLog<K> >(ops);
    std::cout << "K = " << K << "\t shift: " << a << " ms\t circular: " << b
              << " ms\t deque: " << c << " ms\n";
}

int main()
{
    const int OPS = 500000;
    std::cout << OPS << " transactions added to a log that keeps the last K\n\n";
    row<10>(OPS);
    row<50>(OPS);
    row<100>(OPS);
    row<500>(OPS);
    std::cout << "\nSpace: shift and circular use K slots (fixed); deque uses K elements plus block overhead.\n";
    return 0;
}
