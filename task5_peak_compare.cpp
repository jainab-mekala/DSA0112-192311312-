// Task 5 - Peak-hour expression vs lookup table, plus surcharge calculation
// Compatible with Dev-C++ (C++11)

#include <iostream>
#include <iomanip>
using namespace std;

// Method 1: combined expression (peak = 8:00-10:00 and 17:00-20:00, end exclusive)
bool isPeakExpression(int hour)
{
    return (hour >= 8 && hour < 10) || (hour >= 17 && hour < 20);
}

// Method 2: lookup table, one entry per hour 0..23 (1 = peak)
const bool PEAK_TABLE[24] = {
    0,0,0,0,0,0,0,0,   // 0-7
    1,1,                // 8-9
    0,0,0,0,0,0,0,      // 10-16
    1,1,1,              // 17-19
    0,0,0,0             // 20-23
};

bool isPeakTable(int hour)
{
    return PEAK_TABLE[hour];      // caller must pass 0..23
}

long long fareWithSurcharge(long long basePaise, bool peak)
{
    return peak ? basePaise * 120 / 100 : basePaise;
}

int main()
{
    cout << "=== Compare expression and lookup table for hours 0-23 ===" << endl;
    int mismatches = 0;
    for (int h = 0; h < 24; h++)
    {
        bool e = isPeakExpression(h);
        bool t = isPeakTable(h);
        if (e != t) mismatches++;
        if (h == 7 || h == 8 || h == 9 || h == 10 || h == 16 || h == 17 || h == 19 || h == 20)
            cout << "Hour " << setw(2) << h << " : expression=" << e << " table=" << t << endl;
    }
    cout << "Mismatches over all 24 hours: " << mismatches << endl << endl;

    cout << "=== Surcharge calculations (paise) ===" << endl;
    long long bases[3] = {1000, 2000, 3000};
    for (int i = 0; i < 3; i++)
    {
        cout << "Base " << bases[i] << " -> peak " << fareWithSurcharge(bases[i], true)
             << ", off-peak " << fareWithSurcharge(bases[i], false) << endl;
    }
    return 0;
}
