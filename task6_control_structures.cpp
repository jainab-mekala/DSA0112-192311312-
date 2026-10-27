// Task 6 - Control Structures
// Demonstrates if-else, switch, ?:, for, range-based for, while, break, continue
// and the peak-hour boundary tests (10:00 AM and 8:00 PM).
// Compatible with Dev-C++ (C++11)
//
// PSEUDOCODE (fare for one journey)
//   INPUT zoneNumber, hour
//   IF zoneNumber < 1 OR zoneNumber > 3 THEN report "invalid zone" and STOP
//   IF hour < 0 OR hour > 23 THEN report "invalid hour" and STOP
//   SWITCH zoneNumber: 1 -> base = 1000, 2 -> base = 2000, 3 -> base = 3000
//   IF (8 <= hour < 10) OR (17 <= hour < 20) THEN fare = base * 120 / 100
//   ELSE fare = base
//   OUTPUT fare

#include <iostream>
#include <iomanip>
using namespace std;

// ----- if-else -----
bool isPeakHour(int hour)
{
    if (hour >= 8 && hour < 10)
        return true;                 // morning peak
    else if (hour >= 17 && hour < 20)
        return true;                 // evening peak
    else
        return false;
}

// ----- switch -----
int baseFare(int zoneNumber)
{
    switch (zoneNumber)
    {
        case 1:  return 1000;
        case 2:  return 2000;
        case 3:  return 3000;
        default: return -1;          // invalid zone
    }
}

int main()
{
    // ----- boundary tests with a for loop -----
    cout << "=== Boundary tests (for loop) ===" << endl;
    int testHours[] = {7, 8, 9, 10, 16, 17, 19, 20};
    for (int i = 0; i < 8; i++)
    {
        int h = testHours[i];
        // ----- conditional (ternary) operator -----
        const char* label = isPeakHour(h) ? "PEAK" : "off-peak";
        cout << setw(2) << h << ":00  " << label << endl;
    }

    // ----- range-based for loop over zones -----
    cout << endl << "=== Fares per zone at 9:00 and 12:00 (range-based for) ===" << endl;
    int zones[] = {1, 2, 3};
    for (int z : zones)
    {
        int base = baseFare(z);
        cout << "Zone " << z << ": peak " << base * 120 / 100
             << " paise, off-peak " << base << " paise" << endl;
    }

    // ----- while loop with break and continue -----
    cout << endl << "=== Journeys on a card (while, continue, break) ===" << endl;
    long long balance = 8000;                       // Rs 80
    int zoneList[] = {1, 9, 2, 3, 1};               // 9 is an invalid zone
    int i = 0;
    while (i < 5)
    {
        int z = zoneList[i];
        i++;
        int fare = baseFare(z);
        if (fare < 0)
        {
            cout << "Zone " << z << ": invalid, skipped" << endl;
            continue;                               // skip to the next journey
        }
        if (balance - fare < 5000)
        {
            cout << "Zone " << z << ": Rs 50 minimum would be broken, stopping" << endl;
            break;                                  // leave the loop
        }
        balance -= fare;
        cout << "Zone " << z << ": paid " << fare << ", balance " << balance << endl;
    }
    cout << "Final balance: " << balance << " paise" << endl;
    return 0;
}
