// Task 9 - Implement and Test the Fare Logic
// Fare calculator with validation and an automatic expected-vs-actual test table.
// All money is in integer paise. Compatible with Dev-C++ (C++11).
//
// ASSUMPTIONS (not stated in the PDF):
//  1. Peak hours are 8:00-9:59 and 17:00-19:59 (10:00 and 20:00 are off-peak).
//  2. Hour is a whole number 0-23.
//  3. Concession percentages are not specified, so no concession is applied here.

#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

const long long MIN_BALANCE_PAISE = 5000;
const int PEAK_SURCHARGE_PERCENT = 20;

enum class Status { OK, INVALID_ZONE, INVALID_HOUR, INSUFFICIENT_BALANCE };

string statusText(Status s)
{
    switch (s)
    {
        case Status::OK:                   return "OK";
        case Status::INVALID_ZONE:         return "INVALID_ZONE";
        case Status::INVALID_HOUR:         return "INVALID_HOUR";
        case Status::INSUFFICIENT_BALANCE: return "INSUFFICIENT";
    }
    return "?";
}

class SmartCard
{
private:
    long long balancePaise;
public:
    SmartCard(long long opening) : balancePaise(opening) {}
    long long getBalance() const { return balancePaise; }
    bool canTravel(long long fare) const { return balancePaise - fare >= MIN_BALANCE_PAISE; }
    bool deduct(long long fare)
    {
        if (!canTravel(fare)) return false;
        balancePaise -= fare;
        return true;
    }
    bool recharge(long long paise)
    {
        if (paise <= 0) return false;       // negative or zero recharge rejected
        balancePaise += paise;
        return true;
    }
};

bool isPeakHour(int hour)
{
    return (hour >= 8 && hour < 10) || (hour >= 17 && hour < 20);
}

long long baseFare(int zone)
{
    switch (zone)
    {
        case 1: return 1000;
        case 2: return 2000;
        case 3: return 3000;
    }
    return -1;
}

// Calculates the fare and, if everything is valid, deducts it from the card.
Status travel(SmartCard &card, int zone, int hour, long long &fareOut)
{
    fareOut = 0;
    long long base = baseFare(zone);
    if (base < 0) return Status::INVALID_ZONE;
    if (hour < 0 || hour > 23) return Status::INVALID_HOUR;

    long long fare = base;
    if (isPeakHour(hour))
        fare = base * (100 + PEAK_SURCHARGE_PERCENT) / 100;
    fareOut = fare;

    if (!card.deduct(fare)) return Status::INSUFFICIENT_BALANCE;
    return Status::OK;
}

struct TestCase
{
    string name;
    long long startBalance;
    int zone;
    int hour;
    Status expStatus;
    long long expFare;
    long long expBalance;
};

int main()
{
    TestCase tests[] = {
        // name                          balance zone hour  status                          fare  balance
        {"Inner off-peak 12:00",          20000, 1, 12, Status::OK,                  1000, 19000},
        {"Middle off-peak 12:00",         20000, 2, 12, Status::OK,                  2000, 18000},
        {"Outer off-peak 12:00",          20000, 3, 12, Status::OK,                  3000, 17000},
        {"Inner peak 9:00",               20000, 1,  9, Status::OK,                  1200, 18800},
        {"Outer peak 18:00",              20000, 3, 18, Status::OK,                  3600, 16400},
        {"Boundary 8:00 (peak)",          20000, 1,  8, Status::OK,                  1200, 18800},
        {"Boundary 10:00 (off-peak)",     20000, 1, 10, Status::OK,                  1000, 19000},
        {"Boundary 17:00 (peak)",         20000, 1, 17, Status::OK,                  1200, 18800},
        {"Boundary 20:00 (off-peak)",     20000, 1, 20, Status::OK,                  1000, 19000},
        {"Leaves exactly Rs 50",           6000, 1, 12, Status::OK,                  1000,  5000},
        {"Would leave Rs 49.99",           6199, 1, 17, Status::INSUFFICIENT_BALANCE,1200,  6199},
        {"Peak leaves exactly Rs 50",      6200, 1, 17, Status::OK,                  1200,  5000},
        {"Invalid zone 0",                20000, 0, 12, Status::INVALID_ZONE,           0, 20000},
        {"Invalid zone 4",                20000, 4, 12, Status::INVALID_ZONE,           0, 20000},
        {"Invalid hour -1",               20000, 1, -1, Status::INVALID_HOUR,           0, 20000},
        {"Invalid hour 24",               20000, 1, 24, Status::INVALID_HOUR,           0, 20000}
    };
    int count = sizeof(tests) / sizeof(tests[0]);

    cout << "=== TABLE 1: fare tests (paise) ===" << endl;
    cout << left << setw(28) << "Test" << setw(15) << "Exp status" << setw(15) << "Act status"
         << setw(9) << "ExpFare" << setw(9) << "ActFare" << setw(9) << "ExpBal" << setw(9) << "ActBal"
         << "Result" << endl;
    int passed = 0;
    for (int i = 0; i < count; i++)
    {
        SmartCard card(tests[i].startBalance);
        long long fare = 0;
        Status s = travel(card, tests[i].zone, tests[i].hour, fare);
        bool ok = (s == tests[i].expStatus) && (fare == tests[i].expFare)
                  && (card.getBalance() == tests[i].expBalance);
        if (ok) passed++;
        cout << left << setw(28) << tests[i].name
             << setw(15) << statusText(tests[i].expStatus) << setw(15) << statusText(s)
             << setw(9) << tests[i].expFare << setw(9) << fare
             << setw(9) << tests[i].expBalance << setw(9) << card.getBalance()
             << (ok ? "PASS" : "FAIL") << endl;
    }
    cout << "Passed " << passed << " of " << count << endl << endl;

    cout << "=== TABLE 2: recharge validation ===" << endl;
    SmartCard rc(10000);
    long long amounts[] = {500, -500, 0};
    bool expected[] = {true, false, false};
    for (int i = 0; i < 3; i++)
    {
        bool r = rc.recharge(amounts[i]);
        cout << "Recharge " << setw(5) << amounts[i] << " paise: expected "
             << (expected[i] ? "accepted" : "rejected") << ", actual "
             << (r ? "accepted" : "rejected") << ", balance " << rc.getBalance()
             << (r == expected[i] ? "  PASS" : "  FAIL") << endl;
    }

    cout << endl << "=== TABLE 3: five journeys on one card (start Rs 500.00 = 50000 paise) ===" << endl;
    // Manual calculation:
    // J1 Inner  07:00 off-peak = 1000  -> 50000-1000 = 49000
    // J2 Inner  08:00 peak     = 1200  -> 49000-1200 = 47800
    // J3 Middle 09:00 peak     = 2400  -> 47800-2400 = 45400
    // J4 Outer  10:00 off-peak = 3000  -> 45400-3000 = 42400
    // J5 Outer  17:00 peak     = 3600  -> 42400-3600 = 38800
    // Total fares = 1000+1200+2400+3000+3600 = 11200
    int jz[5] = {1, 1, 2, 3, 3};
    int jh[5] = {7, 8, 9, 10, 17};
    long long expF[5] = {1000, 1200, 2400, 3000, 3600};
    long long expB[5] = {49000, 47800, 45400, 42400, 38800};
    SmartCard card5(50000);
    long long totalActual = 0, totalExpected = 0;
    for (int i = 0; i < 5; i++)
    {
        long long fare = 0;
        Status s = travel(card5, jz[i], jh[i], fare);
        totalActual += fare;
        totalExpected += expF[i];
        cout << "J" << i + 1 << " zone " << jz[i] << " hour " << setw(2) << jh[i]
             << " | exp fare " << setw(4) << expF[i] << " act " << setw(4) << fare
             << " | exp bal " << setw(5) << expB[i] << " act " << setw(5) << card5.getBalance()
             << " | " << statusText(s)
             << ((fare == expF[i] && card5.getBalance() == expB[i]) ? "  PASS" : "  FAIL") << endl;
    }
    cout << "Total fares: expected " << totalExpected << ", actual " << totalActual
         << (totalActual == totalExpected ? "  PASS" : "  FAIL") << endl;
    return 0;
}
