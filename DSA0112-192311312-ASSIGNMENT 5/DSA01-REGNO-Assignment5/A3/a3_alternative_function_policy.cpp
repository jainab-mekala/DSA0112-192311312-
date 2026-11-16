// A3 - Alternative concept: payers as function pointers / std::function (a policy chosen at run time).
// Compares how each design stores data such as the cover limit, the space per payer and the time per call.
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <ctime>

struct Bill { double total; };

// ---------------- (A) abstract class ----------------
class Payer {
public:
    virtual ~Payer() = default;
    virtual double amountCovered(const Bill& b) const = 0;
};
class Insurance : public Payer {
    double limit_, coPay_;                             // data lives INSIDE the object
public:
    Insurance(double limit, double coPay) : limit_(limit), coPay_(coPay) {}
    double amountCovered(const Bill& b) const override
    { double s = b.total * (100 - coPay_) / 100; return s < limit_ ? s : limit_; }
};

// ---------------- (B1) plain function pointer: NO place to keep the limit ----------------
// The limit must come from somewhere else: a global, or an extra 'context' argument.
typedef double (*PolicyFn)(const Bill&, const void* context);
struct InsuranceContext { double limit, coPay; };
double insurancePolicy(const Bill& b, const void* context)
{
    const InsuranceContext* c = static_cast<const InsuranceContext*>(context);
    double s = b.total * (100 - c->coPay) / 100;
    return s < c->limit ? s : c->limit;
}

// ---------------- (B2) std::function: the lambda CAPTURES the data ----------------
typedef std::function<double(const Bill&)> Policy;
Policy makeInsurancePolicy(double limit, double coPay)
{
    return [limit, coPay](const Bill& b) {
        double s = b.total * (100 - coPay) / 100;
        return s < limit ? s : limit;
    };
}

volatile double sink = 0;

int main()
{
    std::cout << "sizeof(Insurance object)           = " << sizeof(Insurance) << " bytes (vptr + 2 doubles)\n";
    std::cout << "sizeof(function pointer)           = " << sizeof(PolicyFn) << " bytes (+ separate InsuranceContext "
              << sizeof(InsuranceContext) << ")\n";
    std::cout << "sizeof(std::function<...>)         = " << sizeof(Policy) << " bytes (captured data stored inside or on the heap)\n\n";

    Insurance ins(100000, 20);
    InsuranceContext ctx = { 100000, 20 };
    Policy pol = makeInsurancePolicy(100000, 20);
    const Payer* payer = &ins;
    PolicyFn fn = insurancePolicy;

    const int N = 1000, ROUNDS = 20000;                // 20,000,000 calls each
    std::vector<Bill> bills(N);
    for (int i = 0; i < N; ++i) bills[i].total = 1000 + i * 37 % 150000;

    double a = 0, b = 0, c = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) a += payer->amountCovered(bills[i]);
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) b += fn(bills[i], &ctx);
    clock_t t2 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) c += pol(bills[i]);
    clock_t t3 = clock();
    sink = a + b + c;

    std::cout << "virtual (abstract Payer) : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "function pointer + ctx   : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "std::function + lambda   : " << 1000.0 * (t3 - t2) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "all three agree? " << ((a == b && b == c) ? "yes" : "NO") << " (" << a << ")\n";
    return 0;
}
