
#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>
using namespace std;

// Function to convert text to uppercase
string toUpperCase(string text)
{
    for (int i = 0; i < text.length(); i++)
    {
        text[i] = toupper(text[i]);
    }
    return text;
}

// Check whether the entered zone is valid
bool isValidZone(string zone)
{
    zone = toUpperCase(zone);

    return zone == "ZONE1" ||
           zone == "ZONE2" ||
           zone == "ZONE3" ||
           zone == "AIRPORT";
}

// Check whether the journey is covered by the monthly pass
bool isCoveredByPass(string from, string to,
                     string passZone1, string passZone2)
{
    bool fromCovered =
        (from == passZone1 || from == passZone2);

    bool toCovered =
        (to == passZone1 || to == passZone2);

    return fromCovered && toCovered;
}

// Calculate the normal fare between zones
int calculateBaseFare(string from, string to)
{
    if (from == "AIRPORT" || to == "AIRPORT")
    {
        return 60;
    }

    if (from == to)
    {
        return 10;
    }

    if ((from == "ZONE1" && to == "ZONE2") ||
        (from == "ZONE2" && to == "ZONE1") ||
        (from == "ZONE2" && to == "ZONE3") ||
        (from == "ZONE3" && to == "ZONE2"))
    {
        return 20;
    }

    if ((from == "ZONE1" && to == "ZONE3") ||
        (from == "ZONE3" && to == "ZONE1"))
    {
        return 30;
    }

    return 0;
}

int main()
{
    string from, to;
    string passZone1, passZone2;
    char passChoice, dayChoice, peakChoice;

    int baseFare;
    double finalFare;

    cout << "========================================\n";
    cout << "     CITY METRO SMART-CARD SYSTEM\n";
    cout << "========================================\n";

    // Display available zones
    cout << "\nAvailable Zones:\n";
    cout << "ZONE1  - Zone 1\n";
    cout << "ZONE2  - Zone 2\n";
    cout << "ZONE3  - Zone 3\n";
    cout << "AIRPORT - Airport Zone (Rs.60)\n";

    // Get journey details
    cout << "\nEnter starting zone: ";
    cin >> from;
    from = toUpperCase(from);

    cout << "Enter destination zone: ";
    cin >> to;
    to = toUpperCase(to);

    // Validate zones
    if (!isValidZone(from) || !isValidZone(to))
    {
        cout << "\nError: Invalid zone entered.\n";
        cout << "Please use ZONE1, ZONE2, ZONE3 or AIRPORT.\n";
        return 0;
    }

    // Monthly pass details
    cout << "\nDo you have a monthly pass? (Y/N): ";
    cin >> passChoice;
    passChoice = toupper(passChoice);

    if (passChoice == 'Y')
    {
        cout << "Enter first pass zone: ";
        cin >> passZone1;
        passZone1 = toUpperCase(passZone1);

        cout << "Enter second pass zone: ";
        cin >> passZone2;
        passZone2 = toUpperCase(passZone2);

        if (!isValidZone(passZone1) ||
            !isValidZone(passZone2) ||
            passZone1 == passZone2)
        {
            cout << "\nError: Invalid monthly pass zones.\n";
            cout << "Choose two different valid zones.\n";
            return 0;
        }
    }

    // Calculate base fare
    baseFare = calculateBaseFare(from, to);

    cout << fixed << setprecision(2);

    cout << "\n========================================\n";
    cout << "             FARE DETAILS\n";
    cout << "========================================\n";

    cout << "Starting Zone   : " << from << endl;
    cout << "Destination     : " << to << endl;

    // Monthly pass rule
    if (passChoice == 'Y' &&
        isCoveredByPass(from, to, passZone1, passZone2))
    {
        finalFare = 0;

        cout << "Monthly Pass    : Valid for this journey\n";
        cout << "Base Fare       : Rs. " << baseFare << endl;
        cout << "Pass Benefit    : Unlimited journey coverage\n";
        cout << "Final Fare      : Rs. " << finalFare << endl;
    }
    else
    {
        cout << "Monthly Pass    : Not applicable\n";
        cout << "Base Fare       : Rs. " << baseFare << endl;

        // Airport fare is fixed at Rs.60
        if (from == "AIRPORT" || to == "AIRPORT")
        {
            finalFare = 60;

            cout << "Airport Fare    : Fixed at Rs. 60.00\n";
            cout << "Final Fare      : Rs. " << finalFare << endl;
        }
        else
        {
            // Weekend or weekday selection
            cout << "\nIs today a weekend? (Y/N): ";
            cin >> dayChoice;
            dayChoice = toupper(dayChoice);

            if (dayChoice == 'Y')
            {
                // Weekend discount: 10%
                // Peak surcharge is not applied on weekends
                finalFare = baseFare * 0.90;

                cout << "Weekend Discount: 10%\n";
                cout << "Peak Surcharge  : Not applied\n";
            }
            else
            {
                cout << "Is it peak hour? (Y/N): ";
                cin >> peakChoice;
                peakChoice = toupper(peakChoice);

                if (peakChoice == 'Y')
                {
                    // Peak-hour surcharge: 20%
                    finalFare = baseFare * 1.20;

                    cout << "Peak Surcharge  : 20%\n";
                    cout << "Weekend Discount: Not applied\n";
                }
                else
                {
                    finalFare = baseFare;

                    cout << "Peak Surcharge  : Not applied\n";
                    cout << "Weekend Discount: Not applied\n";
                }
            }

            cout << "Final Fare      : Rs. " << finalFare << endl;
        }
    }

    cout << "========================================\n";
    cout << "       THANK YOU FOR TRAVELLING!\n";
    cout << "========================================\n";

    return 0;
}
