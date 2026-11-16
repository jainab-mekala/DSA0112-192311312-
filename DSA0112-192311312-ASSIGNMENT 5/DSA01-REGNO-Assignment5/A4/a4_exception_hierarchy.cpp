// A4.1 / A4.2 - Errors, exception hierarchy and exception specifications
//
// A4.1 ERROR LIST
//   E1 negative charge (consultation, tests, days, rates ...)   -> NegativeChargeError
//   E2 claim above the policy cover                              -> ClaimError
//   E3 unknown package code in a government scheme               -> UnknownPackageError
//   E4 missing patient record (empty / unknown patient)          -> MissingRecordError
//   E5 memory allocation failure                                 -> std::bad_alloc (standard, not wrapped)
//   E6 text where a number is expected                           -> std::invalid_argument (from std::stod)
//
// EXCEPTION HIERARCHY
//        std::exception
//             |
//        std::runtime_error
//             |
//        BillingError                       (every billing problem; caught as one group)
//        /      |         |           \  (four derived classes)
//  NegativeCharge  ClaimError  UnknownPackage  MissingRecord
//     Error                       Error            Error
//
// A4.2 SPECIFICATIONS
//   may throw    : constructors that validate, Payer::amountCovered(), readAmount(), lookupPatient()
//   must NOT throw (noexcept) : what(), destructors, getters, tryTotal() (catches everything
//                  itself and returns a status), swap/move operations
//   Why destructors must not throw: during stack unwinding destructors run because an exception
//   is already active. If a destructor throws a SECOND exception while the first is unwinding,
//   the program calls std::terminate() - no handler can recover and resources are lost.
#include <iostream>
#include <map>
#include <new>
#include <stdexcept>
#include <string>

class BillingError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
class NegativeChargeError  : public BillingError { public: using BillingError::BillingError; };
class ClaimError           : public BillingError { public: using BillingError::BillingError; };
class UnknownPackageError  : public BillingError { public: using BillingError::BillingError; };
class MissingRecordError   : public BillingError { public: using BillingError::BillingError; };

double checkedCharge(double v)                          // may throw
{
    if (v < 0) throw NegativeChargeError("negative charge: " + std::to_string(v));
    return v;
}
double approveClaim(double billed, double cover)        // may throw
{
    if (billed > cover) throw ClaimError("claim " + std::to_string(billed) + " exceeds cover " + std::to_string(cover));
    return billed;
}
double packageRate(const std::map<std::string, double>& m, const std::string& code)   // may throw
{
    std::map<std::string, double>::const_iterator it = m.find(code);
    if (it == m.end()) throw UnknownPackageError("unknown package code '" + code + "'");
    return it->second;
}
std::string lookupPatient(const std::map<int, std::string>& m, int id)                  // may throw
{
    std::map<int, std::string>::const_iterator it = m.find(id);
    if (it == m.end()) throw MissingRecordError("no patient record for id " + std::to_string(id));
    return it->second;
}
double readAmount(const std::string& text) { return std::stod(text); }                  // may throw invalid_argument

// never throws: converts every failure into a status the caller can test
bool tryApprove(double billed, double cover, std::string& why) noexcept
{
    try {
        approveClaim(billed, cover);
        return true;
    } catch (const std::exception& e) {
        try { why = e.what(); } catch (...) {}          // even assigning a string could throw bad_alloc
        return false;
    } catch (...) {
        return false;
    }
}

struct SafeResource {
    ~SafeResource() noexcept { std::cout << "  [~SafeResource: destructors never throw]\n"; }
    // ~BadResource() { throw std::runtime_error("x"); }   // <-- NEVER: terminate() during unwinding
};

template <typename F>
void attempt(const char* label, F f)                    // runs one failing scenario and reports it
{
    std::cout << label << ": ";
    try {
        SafeResource r;
        f();
        std::cout << "no error\n";
    } catch (const ClaimError& e)       { std::cout << "ClaimError        -> " << e.what() << '\n'; }
    catch (const BillingError& e)       { std::cout << "BillingError(base)-> " << e.what() << '\n'; }   // groups E1,E3,E4
    catch (const std::bad_alloc& e)     { std::cout << "bad_alloc         -> " << e.what() << '\n'; }
    catch (const std::invalid_argument& e) { std::cout << "invalid_argument  -> " << e.what() << '\n'; }
    catch (const std::exception& e)     { std::cout << "std::exception    -> " << e.what() << '\n'; }
}

int main()
{
    std::map<std::string, double> packages; packages["IP"] = 30000;
    std::map<int, std::string> patients;    patients[1] = "Ravi";

    attempt("E1 negative charge ", [&] { checkedCharge(-500); });
    attempt("E2 claim over cover", [&] { approveClaim(60000, 50000); });
    attempt("E3 unknown package ", [&] { packageRate(packages, "XRAY"); });
    attempt("E4 missing record  ", [&] { lookupPatient(patients, 99); });
    attempt("E5 allocation fail ", [&] { double* p = new double[std::size_t(1) << 45]; delete[] p; });
    attempt("E6 text as number  ", [&] { readAmount("abc"); });
    attempt("valid claim        ", [&] { approveClaim(40000, 50000); });

    std::string why;
    bool ok = tryApprove(60000, 50000, why);
    std::cout << "\ntryApprove (noexcept) -> " << (ok ? "approved" : "rejected: " + why) << '\n';
    std::cout << "noexcept(tryApprove)  = " << std::boolalpha << noexcept(tryApprove(1, 2, why)) << '\n';
    return 0;
}
