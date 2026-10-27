// Task 8 - Safe Input and Output
// cin.fail(), cin.clear(), cin.ignore(), getline() + stoi(), iomanip, journey report
// Compatible with Dev-C++ (C++11, compile with -std=c++11)

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <stdexcept>
using namespace std;

// Remove the rest of the bad line from the input buffer
void clearInputLine()
{
    cin.clear();                                               // reset the error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n');       // discard characters up to Enter
}

// Method 1: cin >> with fail() recovery. Returns false only if input ended (Ctrl+Z / EOF).
bool readIntCin(const string& prompt, int minV, int maxV, int &result)
{
    while (true)
    {
        cout << prompt;
        int value;
        cin >> value;
        if (cin.eof()) return false;
        if (cin.fail())
        {
            cout << "  Not a number. Try again." << endl;
            clearInputLine();
            continue;
        }
        clearInputLine();                                      // remove leftover text (e.g. "5abc")
        if (value < minV || value > maxV)
        {
            cout << "  Out of range (" << minV << " to " << maxV << "). Try again." << endl;
            continue;
        }
        result = value;
        return true;
    }
}

// Method 2: getline() + stoi() - reads the whole line, then converts
bool readIntLine(const string& prompt, int minV, int maxV, int &result)
{
    while (true)
    {
        cout << prompt;
        string line;
        if (!getline(cin, line)) return false;
        try
        {
            size_t used = 0;
            int value = stoi(line, &used);
            if (used != line.size())                           // text left over such as "9abc"
            {
                cout << "  Extra characters found. Try again." << endl;
                continue;
            }
            if (value < minV || value > maxV)
            {
                cout << "  Out of range (" << minV << " to " << maxV << "). Try again." << endl;
                continue;
            }
            result = value;
            return true;
        }
        catch (const invalid_argument&)
        {
            cout << "  Not a number. Try again." << endl;
        }
        catch (const out_of_range&)
        {
            cout << "  Number too large. Try again." << endl;
        }
    }
}

bool isPeakHour(int hour)
{
    return (hour >= 8 && hour < 10) || (hour >= 17 && hour < 20);
}

void printMoney(long long paise)
{
    cout << "Rs " << right << setw(8) << fixed << setprecision(2) << paise / 100.0;   // display only
}

int main()
{
    cout << "=== Metro Journey (safe input demo) ===" << endl;

    int startRupees, zone, hour;
    if (!readIntCin("Enter card balance in whole rupees (60 to 100000): ", 60, 100000, startRupees)) return 0;
    if (!readIntCin("Enter zone (1=Inner, 2=Middle, 3=Outer): ", 1, 3, zone)) return 0;
    if (!readIntLine("Enter hour of travel (0-23): ", 0, 23, hour)) return 0;

    long long balance = startRupees * 100LL;
    long long base = zone * 1000LL;                            // 1000, 2000 or 3000 paise
    bool peak = isPeakHour(hour);
    long long fare = peak ? base * 120 / 100 : base;
    bool accepted = (balance - fare >= 5000);
    long long after = accepted ? balance - fare : balance;

    cout << endl << "-------- JOURNEY REPORT --------" << endl;
    cout << left << setw(18) << "Zone"          << ": " << zone << endl;
    cout << left << setw(18) << "Hour"          << ": " << hour << ":00" << endl;
    cout << left << setw(18) << "Peak hour"     << ": " << (peak ? "Yes" : "No") << endl;
    cout << left << setw(18) << "Opening balance" << ": "; printMoney(balance); cout << endl;
    cout << left << setw(18) << "Fare"          << ": "; printMoney(fare);    cout << endl;
    cout << left << setw(18) << "Status"        << ": " << (accepted ? "ACCEPTED" : "REJECTED (Rs 50 minimum)") << endl;
    cout << left << setw(18) << "Closing balance" << ": "; printMoney(after);   cout << endl;
    cout << "--------------------------------" << endl;
    return 0;
}
