// Task 5 - Expressions and Operator Precedence : PREDICTION PROGRAM
// (Replace/compare with the exact program supplied in your assignment PDF.)
// STEP 1: Do NOT run it. Write your predicted output first.

#include <iostream>
using namespace std;

int main()
{
    int a = 5, b = 3, c = 2;
    cout << "1: " << a + b * c << endl;
    cout << "2: " << (a + b) * c << endl;
    cout << "3: " << a - b - c << endl;
    cout << "4: " << 10 + 20 % 3 * 2 << endl;

    int x = 5;
    int y = x++;
    int z = ++x;
    cout << "5: " << y << " " << z << " " << x << endl;

    cout << "6: " << -7 / 2 << " " << -7 % 2 << endl;
    cout << "7: " << 7 / -2 << " " << 7 % -2 << endl;

    int hour = 9;
    bool peak = hour >= 8 && hour < 10 || hour >= 17 && hour < 20;
    cout << "8: " << peak << endl;

    cout << "9: " << 1000 * 20 / 100 << " " << 1000 / 100 * 20 << " " << 1000 / (100 * 20) << endl;
    return 0;
}
