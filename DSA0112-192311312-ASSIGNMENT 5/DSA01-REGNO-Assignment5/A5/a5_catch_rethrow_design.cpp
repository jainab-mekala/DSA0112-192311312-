// A5.1 / A5.2 - Where to catch, log + rethrow with 'throw;', report at the top; RAII on unwinding.
//
// A5.1 WHERE EACH EXCEPTION IS HANDLED
//   +-----------------------+------------------------------------------------------------+
//   | LOW level  (validate) | THROWS NegativeChargeError / ClaimError / UnknownPackageError |
//   |                       | -> does not catch: it cannot decide what to do                |
//   | MIDDLE level (process)| catches BillingError&, LOGS "[LOG] ...", rethrows with throw; |
//   |                       | (same object, same dynamic type; nothing is hidden)           |
//   | TOP level (main loop) | catches ClaimError (reject claim), BillingError (report),     |
//   |                       | std::exception, catch(...) -> reports to the user, goes on    |
//   |                       | with the NEXT bill: no bill is lost because of another's error|
//   +-----------------------+------------------------------------------------------------+
//
// A5.2 STACK UNWINDING: when throw executes, the runtime leaves each function from the
//   inside out and calls the destructor of every fully constructed local object on the way.
//   Objects owning resources (unique_ptr, vector, ScopedLedger) release them in their destructors,
//   so no catch block has to remember to free anything.
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class BillingError : public std::runtime_error { public: using std::runtime_error::runtime_error; };
class NegativeChargeError : public BillingError { public: using BillingError::BillingError; };
class ClaimError          : public BillingError { public: using BillingError::BillingError; };
class UnknownPackageError : public BillingError { public: using BillingError::BillingError; };

int liveResources = 0;                                   // proves that nothing leaks

class ScopedLedger {                                     // RAII: opens in the constructor, closes in the destructor
    std::string name_;
    std::unique_ptr<std::vector<double> > entries_;      // heap memory owned by a smart pointer
public:
    explicit ScopedLedger(const std::string& name) : name_(name), entries_(new std::vector<double>)
    { ++liveResources; std::cout << "    [open ledger " << name_ << "]\n"; }
    ~ScopedLedger() { --liveResources; std::cout << "    [close ledger " << name_ << "]\n"; }
    ScopedLedger(const ScopedLedger&) = delete;
    ScopedLedger& operator=(const ScopedLedger&) = delete;
    void add(double v) { entries_->push_back(v); }
};

// LOW level: detects the error and throws
double validate(double charge, double cover, const std::string& code)
{
    if (charge < 0) throw NegativeChargeError("negative charge");
    if (charge > cover) throw ClaimError("claim exceeds cover");
    if (code != "IP" && code != "OP") throw UnknownPackageError("unknown package " + code);
    return charge;
}

// MIDDLE level: logs and rethrows
double process(double charge, double cover, const std::string& code)
{
    ScopedLedger ledger("bill-" + code);                 // destroyed during unwinding if validate throws
    try {
        double v = validate(charge, cover, code);
        ledger.add(v);
        return v;
    } catch (const BillingError& e) {
        std::cout << "    [LOG] " << e.what() << " (rethrowing)\n";
        throw;                                           // rethrow the SAME exception object
    }
}

int main()
{
    struct Case { double charge, cover; const char* code; };
    Case cases[] = { {40000, 50000, "IP"}, {60000, 50000, "IP"}, {-5, 50000, "OP"}, {100, 500, "XYZ"}, {900, 1000, "OP"} };

    for (int i = 0; i < 5; ++i) {
        std::cout << "bill " << i + 1 << ":\n";
        try {
            double paid = process(cases[i].charge, cases[i].cover, cases[i].code);
            std::cout << "  approved " << paid << '\n';
        } catch (const ClaimError& e) {                  // TOP level: final reporting to the user
            std::cout << "  REJECTED (claim): " << e.what() << '\n';
        } catch (const BillingError& e) {
            std::cout << "  ERROR (billing): " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "  ERROR (system): " << e.what() << '\n';
        } catch (...) {
            std::cout << "  ERROR: unknown\n";
        }
    }
    std::cout << "\nledgers still open after all bills: " << liveResources << "   (0 = nothing leaked)\n";
    return 0;
}
