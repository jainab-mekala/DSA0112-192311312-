// Task 6 - Stage 5: demonstrate renting each type, returning with a late charge, listing.
#include "t6_vehicle_fleet.h"

void report(const char* what, bool ok) { std::cout << "  " << what << " -> " << (ok ? "OK" : "REFUSED") << '\n'; }

void giveBack(Fleet& fleet, const std::string& reg, int daysUsed, double energy)
{
    double amount = fleet.giveBack(reg, daysUsed, energy);
    if (amount < 0) std::cout << "  " << reg << " was not rented\n";
    else std::cout << "  returned " << std::left << std::setw(10) << reg << " used " << daysUsed
                   << " day(s), energy " << energy << " -> charge Rs." << amount << '\n';
}

int main()
{
    std::cout << std::fixed << std::setprecision(2);
    const Date today(15, 11, 2026);

    Fleet fleet;
    fleet.add(new Bicycle("BIC-001", 150));
    fleet.add(new TwoWheeler("TN30-1234", 400, 70, Date(15, 12, 2026)));
    fleet.add(new Car("TN30-5678", 1500, 60, Date(10, 11, 2026), 5));          // service already overdue
    fleet.add(new ElectricCar("TN30-EV01", 2200, 80, Date(20, 1, 2027), 5, 40.0));

    std::cout << "FLEET AT START\n";
    fleet.list(today);

    std::cout << "\nRENTING (2 days booked each)\n";
    report("rent BIC-001   ", fleet.rent("BIC-001", 2));
    report("rent TN30-1234 ", fleet.rent("TN30-1234", 2));
    report("rent TN30-5678 ", fleet.rent("TN30-5678", 2));
    report("rent TN30-EV01 ", fleet.rent("TN30-EV01", 2));
    report("rent BIC-001 again (already rented)", fleet.rent("BIC-001", 1));
    report("rent XX-0000 (unknown)", fleet.rent("XX-0000", 1));

    std::cout << "\nFLEET WHILE RENTED\n";
    fleet.list(today);

    std::cout << "\nRETURNS (booked 2 days)\n";
    giveBack(fleet, "BIC-001",   2, 0);        // on time
    giveBack(fleet, "TN30-1234", 3, 4.5);      // 1 day late, 4.5 litres
    giveBack(fleet, "TN30-5678", 4, 20);       // 2 days late, 20 litres
    giveBack(fleet, "TN30-EV01", 3, 15);       // 1 day late, 15 kWh
    giveBack(fleet, "TN30-EV01", 1, 0);        // not rented any more

    std::cout << "\nFLEET AT END\n";
    fleet.list(today);
    return 0;
}
