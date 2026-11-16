// B4.2 - Check resource safety: a batch run in which exceptions are thrown AND caught.
// Compile and run under the address sanitizer (Linux / WSL / MSYS2; not the old MinGW of Dev-C++):
//     g++ -std=c++17 -g -fsanitize=address,undefined -Wall -Wextra b4_sanitizer_run.cpp -o b4_asan
//     ASAN_OPTIONS=detect_leaks=1:allocator_may_return_null=1 ./b4_asan
// A CLEAN report = the program prints its summary and AddressSanitizer prints NOTHING afterwards
// (no leak, no invalid access, no mismatch). Save a screenshot of the terminal as evidence.
// NOTE: the line "WARNING: AddressSanitizer failed to allocate 0x1000000000000 bytes" is expected: it is
//       the deliberate impossible allocation that this program catches as std::bad_alloc. It is not an error.
#include "billing.h"
#include <memory>
#include <new>
#include <vector>

struct Order { const char* kind; const char* patient; double a, b, c, d; };

std::unique_ptr<Bill> make(const Order& o)               // may throw: nothing leaks if it does
{
    std::string kind = o.kind;
    if (kind == "OP") return std::unique_ptr<Bill>(new OutPatientBill(o.patient, o.a, o.b));
    if (kind == "IP") return std::unique_ptr<Bill>(new InPatientBill(o.patient, int(o.a), o.b, o.c, o.d));
    if (kind == "DC") return std::unique_ptr<Bill>(new DayCareBill(o.patient, "DC-CATARACT", o.a));
    if (kind == "ER") return std::unique_ptr<Bill>(new EmergencyBill(o.patient, o.a, o.b, o.c));
    throw BillingError("unknown bill kind '" + kind + "'");
}

int main()
{
    Order orders[] = {
        { "OP", "Ravi",  500, 1200, 0, 0 },
        { "IP", "Meena", 3, 2500, 40000, 5000 },
        { "DC", "Kiran", 25000, 0, 0, 0 },
        { "ER", "Sara",  3000, 2.5, 800, 0 },
        { "OP", "Bad1", -500, 1200, 0, 0 },              // NegativeChargeError
        { "OP", "",      100, 100, 0, 0 },               // MissingRecordError
        { "XX", "Bad3",  1, 1, 1, 1 },                   // BillingError (unknown kind)
        { "IP", "Big",   30, 5000, 900000, 20000 }       // valid bill, too large for the insurance cover
    };

    std::map<std::string, double> rates; rates["OP"] = 1000; rates["IP"] = 30000;     // no ER, no DC
    SelfPay self; Insurance ins(100000, 20); GovernmentScheme gov(rates);
    const Payer* payers[3] = { &self, &ins, &gov };

    std::vector< std::unique_ptr<Bill> > accepted;
    int built = 0, buildRejected = 0, settled = 0, claimRejected = 0, otherErrors = 0;

    for (std::size_t i = 0; i < sizeof(orders) / sizeof(orders[0]); ++i) {
        try {
            accepted.push_back(make(orders[i]));
            ++built;
        } catch (const BillingError& e) {
            ++buildRejected;
            std::cout << "build rejected (" << orders[i].patient << "): " << e.what() << '\n';
        }
    }
    for (std::size_t i = 0; i < accepted.size(); ++i) {
        for (int p = 0; p < 3; ++p) {
            try {
                settle(*accepted[i], *payers[p]);
                ++settled;
            } catch (const ClaimError& e) {
                ++claimRejected;
                std::cout << "claim rejected (" << accepted[i]->patient() << ", " << payers[p]->name() << "): " << e.what() << '\n';
            } catch (const BillingError& e) {
                ++otherErrors;
                std::cout << "billing error  (" << accepted[i]->patient() << ", " << payers[p]->name() << "): " << e.what() << '\n';
            }
        }
    }

    try { std::stod("12,5 rupees"); std::cout << "(stod parsed the leading '12')\n"; }
    catch (const std::invalid_argument& e) { ++otherErrors; std::cout << "invalid number: " << e.what() << '\n'; }
    try { std::stod("rupees"); }
    catch (const std::invalid_argument& e) { ++otherErrors; std::cout << "invalid number: " << e.what() << '\n'; }

    try {                                                 // allocation failure, thrown and caught
#ifdef __SANITIZE_ADDRESS__
        double* huge = new (std::nothrow) double[std::size_t(1) << 45];
        if (!huge) throw std::bad_alloc();
#else
        double* huge = new double[std::size_t(1) << 45];
#endif
        delete[] huge;
    } catch (const std::bad_alloc&) { ++otherErrors; std::cout << "allocation failure caught (bad_alloc)\n"; }

    std::cout << "\nSUMMARY: bills built " << built << ", build rejected " << buildRejected
              << ", settlements " << settled << ", claims rejected " << claimRejected
              << ", other errors " << otherErrors << '\n';
    std::cout << "No bill was lost: the " << built << " valid bills are still held by the vector and are released when it is destroyed.\n";
    return 0;   // accepted (vector<unique_ptr>) is destroyed here; ASAN/LSan check the heap afterwards
}
