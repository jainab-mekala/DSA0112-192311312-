// B2 - Engineer error conditions. Uses billing.h. Every scenario below is run inside its own
// try block in a loop: the program must keep running after each failure, and the Tracer
// destructor messages prove that all objects are destroyed.
//
// Scenarios: 1 negative charge   2 claim above cover   3 unknown package code
//            4 text where a number is expected (stod)  5 missing patient record
//            6 memory allocation failure (bad_alloc)   7 a non-standard exception (caught by catch(...))
//
// Address sanitizer note: AddressSanitizer replaces operator new and ABORTS on an impossible
// allocation instead of throwing std::bad_alloc. So when compiled with -fsanitize=address
// (the macro __SANITIZE_ADDRESS__ is defined) scenario 6 uses new(std::nothrow) and throws
// std::bad_alloc itself. Run it with   ASAN_OPTIONS=allocator_may_return_null=1   (Linux/WSL/MSYS2).
#include "billing.h"
#include <new>

struct Tracer {                                          // prints in constructor and destructor
    std::string n;
    explicit Tracer(const std::string& s) : n(s) { std::cout << "  [+" << n << "]"; }
    ~Tracer() { std::cout << " [-" << n << "]"; }
};

void scenario(int id)
{
    Tracer outerObject("scenario" + std::to_string(id));
    switch (id) {
        case 1: { Tracer t("bill"); OutPatientBill b("Ravi", -500, 1200); b.print(); break; }
        case 2: { Tracer t("bill"); InPatientBill b("Meena", 3, 2500, 40000, 5000);
                  Insurance ins(10000, 20); settle(b, ins); break; }
        case 3: { Tracer t("bill"); EmergencyBill b("Sara", 3000, 2.5, 800);
                  std::map<std::string, double> rates; rates["OP"] = 1000;
                  GovernmentScheme gov(rates); settle(b, gov); break; }
        case 4: { Tracer t("parse"); double v = std::stod("abc"); std::cout << v; break; }
        case 5: { Tracer t("bill"); OutPatientBill b("", 100, 100); b.print(); break; }
        case 6: { Tracer t("alloc");
#ifdef __SANITIZE_ADDRESS__
                  double* huge = new (std::nothrow) double[std::size_t(1) << 45];
                  if (!huge) throw std::bad_alloc();
#else
                  double* huge = new double[std::size_t(1) << 45];      // impossible request -> std::bad_alloc
#endif
                  huge[0] = 1; delete[] huge; break; }
        case 7: { Tracer t("odd"); throw 42; }
        default: break;
    }
}

int main()
{
    const char* names[] = { "", "negative charge", "claim above cover", "unknown package", "text as number",
                            "missing patient", "memory allocation failure", "non-standard exception" };
    int handled = 0;
    for (int id = 1; id <= 7; ++id) {
        std::cout << id << ". " << names[id] << '\n';
        try {
            scenario(id);
        } catch (const ClaimError& e) {                  // most specific first
            std::cout << "\n   -> ClaimError: " << e.what() << '\n';
        } catch (const BillingError& e) {                // negative, unknown package, missing record
            std::cout << "\n   -> BillingError family: " << e.what() << '\n';
        } catch (const std::bad_alloc& e) {
            std::cout << "\n   -> std::bad_alloc: " << e.what() << '\n';
        } catch (const std::invalid_argument& e) {
            std::cout << "\n   -> std::invalid_argument: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "\n   -> other std::exception: " << e.what() << '\n';
        } catch (...) {                                  // last line of defence: the program keeps running
            std::cout << "\n   -> catch(...): unknown exception, program continues\n";
        }
        ++handled;
    }
    std::cout << "\nall " << handled << " scenarios handled; the program is still running.\n";

    // second kind of text error: a number out of range for double
    try { std::stod("1e999"); }
    catch (const std::out_of_range& e) { std::cout << "std::stod(\"1e999\") -> std::out_of_range: " << e.what() << '\n'; }
    return 0;
}
