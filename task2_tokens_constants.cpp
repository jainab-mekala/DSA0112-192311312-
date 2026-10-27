// Task 2 - Tokens, Keywords, Identifiers and Constants
// City Metro Smart-Card System - fare constants and zones
// Compatible with Dev-C++ (C++11). Money is stored in integer PAISE.

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

// ============ 1. MACROS (#define): text replacement, NO type, NO scope ============
#define MACRO_INNER_FARE   1000          // looks like a constant...
#define MACRO_PEAK_PERCENT 100 + 20      // DANGEROUS: no brackets, see main()

// ============ 2. TYPED CONSTANTS (const) ============
const int       FARE_INNER_PAISE  = 1000;     // Rs 10
const int       FARE_MIDDLE_PAISE = 2000;     // Rs 20
const int       FARE_OUTER_PAISE  = 3000;     // Rs 30
const int       PEAK_SURCHARGE_PERCENT = 20;  // 20%
const long long MIN_BALANCE_PAISE = 5000;     // Rs 50 must remain on card

// ============ 3. constexpr: value known at COMPILE time ============
constexpr int PEAK_MORNING_START = 8;     // 8:00 AM
constexpr int PEAK_MORNING_END   = 10;    // 10:00 AM (exclusive)
constexpr int PEAK_EVENING_START = 17;    // 5:00 PM
constexpr int PEAK_EVENING_END   = 20;    // 8:00 PM (exclusive)
constexpr int ZONE_COUNT         = 3;

// A constexpr function can also run at compile time (C++11: one return only)
constexpr long long withSurcharge(long long basePaise, int percent)
{
    return basePaise * (100 + percent) / 100;
}

// The compiler checks this while compiling. If the formula is wrong, it will not build.
static_assert(withSurcharge(1000, 20) == 1200, "Rs 10 + 20% must be Rs 12");

// ============ 4. ENUMERATIONS ============
// Old-style enum: names leak into the surrounding scope and convert to int silently
enum OldZone { OLD_INNER = 1, OLD_MIDDLE = 2, OLD_OUTER = 3 };

// enum class: scoped (Zone::Inner) and strongly typed (no silent conversion to int)
enum class Zone { Inner = 1, Middle = 2, Outer = 3 };

// ============ 5. Functions using the constants ============
int baseFare(Zone z)
{
    switch (z)
    {
        case Zone::Inner:  return FARE_INNER_PAISE;
        case Zone::Middle: return FARE_MIDDLE_PAISE;
        case Zone::Outer:  return FARE_OUTER_PAISE;
    }
    return 0;
}

string zoneName(Zone z)
{
    switch (z)
    {
        case Zone::Inner:  return "Inner";
        case Zone::Middle: return "Middle";
        case Zone::Outer:  return "Outer";
    }
    return "Unknown";
}

void printMoney(long long paise)
{
    cout << "Rs " << paise / 100 << "."
         << setfill('0') << setw(2) << paise % 100 << setfill(' ');
}

int main()
{
    // ---- Part A: the five token categories in ONE statement ----
    // long long fare = 1000 * 120 / 100 ;
    // keywords: long, long | identifier: fare | operators: = * / | constants: 1000,120,100 | punctuators: ;
    long long fare = 1000 * 120 / 100;
    cout << "Part A - fare computed by the statement above: ";
    printMoney(fare);
    cout << endl << endl;

    // ---- Part B: fare table using typed constants and enum class ----
    cout << "Part B - Fare table (using const + enum class)" << endl;
    Zone zones[ZONE_COUNT] = { Zone::Inner, Zone::Middle, Zone::Outer };
    for (int i = 0; i < ZONE_COUNT; i++)
    {
        int base = baseFare(zones[i]);
        cout << left << setw(8) << zoneName(zones[i])
             << " zone number = " << static_cast<int>(zones[i])
             << " | off-peak: ";
        printMoney(base);
        cout << " | peak: ";
        printMoney(withSurcharge(base, PEAK_SURCHARGE_PERCENT));
        cout << endl;
    }
    cout << "Minimum balance to keep: "; printMoney(MIN_BALANCE_PAISE);
    cout << endl << endl;

    // ---- Part C: macro versus typed constant ----
    cout << "Part C - Macro pitfall" << endl;
    long long wrong   = 1000 * MACRO_PEAK_PERCENT / 100;      // becomes 1000 * 100 + 20 / 100
    long long correct = 1000 * (100 + PEAK_SURCHARGE_PERCENT) / 100;
    cout << "Using macro  (100 + 20 as text): " << wrong << " paise  <-- WRONG" << endl;
    cout << "Using const  (value 120)       : " << correct << " paise  <-- correct" << endl << endl;

    // ---- Part D: enum versus enum class ----
    cout << "Part D - enum versus enum class" << endl;
    int oldValue = OLD_MIDDLE;                       // allowed: silent conversion
    cout << "Old enum OLD_MIDDLE as int      : " << oldValue << endl;
    // int bad = Zone::Middle;                       // ERROR: enum class does not convert silently
    int newValue = static_cast<int>(Zone::Middle);   // must be explicit
    cout << "enum class Zone::Middle via cast: " << newValue << endl << endl;

    // ---- Part E: kinds of literal constants ----
    cout << "Part E - Literal constants" << endl;
    cout << "Integer literal   : " << 1000 << endl;
    cout << "long long literal : " << 3000000000LL << " (suffix LL)" << endl;
    cout << "Floating literal  : " << 0.20 << endl;
    cout << "Character literal : " << 'Z' << endl;
    cout << "String literal    : " << "Inner" << endl;
    cout << "Boolean literal   : " << boolalpha << true << endl;
    return 0;
}
