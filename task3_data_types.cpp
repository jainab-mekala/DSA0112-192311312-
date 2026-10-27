// Task 3 - Data Types and Type Compatibility
// Experiment: sizes, limits, paise vs double, conversions and overflow
// Compatible with Dev-C++ (C++11)

#include <iostream>
#include <iomanip>
#include <climits>
#include <cfloat>
using namespace std;

int main()
{
    // ---------- Part A: sizeof of common types ----------
    cout << "=== Part A: sizeof (bytes on THIS compiler) ===" << endl;
    cout << "char          : " << sizeof(char) << endl;
    cout << "bool          : " << sizeof(bool) << endl;
    cout << "short         : " << sizeof(short) << endl;
    cout << "int           : " << sizeof(int) << endl;
    cout << "long          : " << sizeof(long) << endl;
    cout << "long long     : " << sizeof(long long) << endl;
    cout << "unsigned int  : " << sizeof(unsigned int) << endl;
    cout << "float         : " << sizeof(float) << endl;
    cout << "double        : " << sizeof(double) << endl << endl;

    // ---------- Part B: limits from <climits> ----------
    cout << "=== Part B: limits ===" << endl;
    cout << "SHRT_MAX  = " << SHRT_MAX << endl;
    cout << "INT_MIN   = " << INT_MIN << endl;
    cout << "INT_MAX   = " << INT_MAX << endl;
    cout << "UINT_MAX  = " << UINT_MAX << endl;
    cout << "UCHAR_MAX = " << UCHAR_MAX << endl;
    cout << "LLONG_MAX = " << LLONG_MAX << endl << endl;

    // ---------- Part C: integer paise vs double rupees ----------
    cout << "=== Part C: paise (integer) vs rupees (double) ===" << endl;
    cout << setprecision(17);
    double a = 0.1, b = 0.2;
    cout << "0.1 + 0.2 = " << a + b << endl;
    cout << "(0.1 + 0.2 == 0.3) is " << boolalpha << ((a + b) == 0.3) << endl;

    double rupees = 4.35;
    int paiseTruncated = static_cast<int>(rupees * 100);          // truncates
    int paiseRounded   = static_cast<int>(rupees * 100 + 0.5);    // rounds
    cout << "4.35 * 100 = " << rupees * 100 << endl;
    cout << "static_cast<int>(4.35 * 100)       = " << paiseTruncated << " paise" << endl;
    cout << "static_cast<int>(4.35 * 100 + 0.5) = " << paiseRounded << " paise" << endl;

    int fareInt = 1200;        // Rs 12.00 stored as integer paise: exact
    cout << "Integer fare 1200 paise = Rs " << fareInt / 100 << "." << fareInt % 100 << "0"
         << " (exact)" << endl << endl;
    cout << setprecision(6);

    // ---------- Part D: type conversions ----------
    cout << "=== Part D: conversions ===" << endl;
    int fare = 1000;
    double widened = fare;                       // int -> double (safe)
    double precise = 2500.75;
    int narrowed = static_cast<int>(precise);    // double -> int (fraction lost)
    cout << "int 1000 -> double      : " << widened << endl;
    cout << "double 2500.75 -> int   : " << narrowed << endl;
    long long big = 3000000000LL;
    int shrunk = static_cast<int>(big);          // value does not fit (implementation-defined)
    cout << "long long 3000000000 -> int: " << shrunk << " (data lost)" << endl << endl;

    // ---------- Part E: overflow ----------
    cout << "=== Part E: overflow ===" << endl;
    unsigned char small = 255;
    small = small + 1;                           // unsigned wraps around (defined behaviour)
    cout << "unsigned char 255 + 1 = " << static_cast<int>(small) << endl;

    unsigned int u = INT_MAX;
    u = u + 1;
    cout << "unsigned INT_MAX + 1  = " << u << endl;

    long long journeys = 3000000;
    long long fareOuterPeak = 3600;              // paise
    long long total = journeys * fareOuterPeak;  // computed safely in long long
    cout << "3,000,000 journeys x 3600 paise = " << total << " paise" << endl;
    cout << "INT_MAX                         = " << INT_MAX << endl;
    cout << "Would this fit in an int?        " << boolalpha << (total <= INT_MAX) << endl;
    cout << "(Signed int overflow is undefined behaviour, so it is NOT executed.)" << endl;
    return 0;
}
