#include <iostream>
#include <iomanip>
#include <climits>
using namespace std;

int main() {
    const long long farePaise = 1200; // Rs.12 per journey
    const long long counts[] = {20000, 500000, 3000000};
    cout << fixed << setprecision(2);
    cout << "Journey totals using Rs.12 per journey\n";
    for (int i = 0; i < 3; ++i) {
        long long n = counts[i];
        long long integerTotalPaise = n * farePaise;
        double doubleTotalRupees = n * 12.0;
        cout << "Journeys: " << n
             << " | integer paise total: " << integerTotalPaise
             << " | rupee total: " << doubleTotalRupees << '\n';
    }
    long long networkTotalPaise = 3000000LL * farePaise;
    cout << "INT_MAX: " << INT_MAX << '\n';
    cout << "Network total paise: " << networkTotalPaise << '\n';
    cout << "Fits in int? " << (networkTotalPaise <= INT_MAX ? "Yes" : "No") << '\n';
    cout << "Use long long for paise totals to avoid int overflow.\n";
    cout << "Record actual compiler warnings from your own compiler; warnings vary by compiler.\n";
    return 0;
}
