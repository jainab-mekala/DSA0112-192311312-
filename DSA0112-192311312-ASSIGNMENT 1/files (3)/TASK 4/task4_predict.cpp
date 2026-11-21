// Task 4 - Control Implicit Conversions : PREDICTION PROGRAM
// (Replace/compare with the exact program supplied in your assignment PDF.)
// STEP 1: Do NOT run it. Write your predicted output on paper first.

#include <iostream>
using namespace std;

int main()
{
    int totalPaise = 7000;
    int journeys = 3;

    double average1 = totalPaise / journeys;     // line A
    double average2 = totalPaise / 3.0;          // line B
    cout << "A: " << average1 << endl;
    cout << "B: " << average2 << endl;

    char zoneCode = 'A';
    int next = zoneCode + 1;                     // line C
    cout << "C: " << next << " " << static_cast<char>(next) << endl;

    int balance = 50;
    bool hasBalance = balance;                   // line D
    int zero = 0;
    bool noBalance = zero;
    cout << "D: " << hasBalance << " " << noBalance << endl;

    unsigned int cardsIssued = 5;
    int cardsUsed = 8;
    cout << "E: " << cardsIssued - cardsUsed << endl;          // line E
    if (cardsIssued - cardsUsed > 0)
        cout << "F: more cards issued than used" << endl;       // line F
    else
        cout << "F: no cards left" << endl;

    int surcharge = 1000 * 20 / 100;
    int wrongSurcharge = 1000 * (20 / 100);                     // line G
    cout << "G: " << surcharge << " " << wrongSurcharge << endl;
    return 0;
}
