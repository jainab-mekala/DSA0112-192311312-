// Task 4 - Control Implicit Conversions : CORRECTED PROGRAM
// Every conversion is now written explicitly so the intention is clear.

#include <iostream>
using namespace std;

int main()
{
    int totalPaise = 7000;
    int journeys = 3;

    // A: convert BEFORE dividing so the fraction is kept
    double average1 = static_cast<double>(totalPaise) / journeys;
    double average2 = totalPaise / 3.0;
    cout << "A: " << average1 << endl;
    cout << "B: " << average2 << endl;

    // C: say clearly that a char is used as a number
    char zoneCode = 'A';
    int next = static_cast<int>(zoneCode) + 1;
    cout << "C: " << next << " " << static_cast<char>(next) << endl;

    // D: compare explicitly instead of relying on int -> bool
    int balance = 50;
    bool hasBalance = (balance != 0);
    int zero = 0;
    bool noBalance = (zero != 0);
    cout << "D: " << hasBalance << " " << noBalance << endl;

    // E/F: use signed types for values that can go below zero
    int cardsIssued = 5;
    int cardsUsed = 8;
    cout << "E: " << cardsIssued - cardsUsed << endl;
    if (cardsIssued - cardsUsed > 0)
        cout << "F: more cards issued than used" << endl;
    else
        cout << "F: no cards left" << endl;

    // G: percentage maths - multiply first and divide last (integer paise),
    // or do the whole calculation in double and convert once at the end
    int surcharge = 1000 * 20 / 100;
    int surchargeDouble = static_cast<int>(1000 * (20.0 / 100.0));
    cout << "G: " << surcharge << " " << surchargeDouble << endl;
    return 0;
}
