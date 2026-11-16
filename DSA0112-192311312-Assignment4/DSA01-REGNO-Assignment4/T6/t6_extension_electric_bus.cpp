// Task 6 - Stage 5: add a NEW vehicle type (ElectricBus) WITHOUT changing any existing class.
// t6_vehicle_fleet.h is included exactly as it is: Vehicle, MotorVehicle, Fleet ... are untouched.
#include "t6_vehicle_fleet.h"

// ---------------- NEW CODE: everything needed for the new type is here ----------------
const double DRIVER_FEE_PER_DAY = 1000.0;

class ElectricBus : public MotorVehicle {
    int    seats_;
    double batteryKWh_;
public:
    ElectricBus(const std::string& regNo, double dailyRate, double level, const Date& serviceDue,
                int seats, double batteryKWh)
        : MotorVehicle(regNo, dailyRate, level, serviceDue), seats_(seats), batteryKWh_(batteryKWh) {}
    std::string type() const override { return "Electric bus"; }
    double charge(int daysUsed, int daysBooked, double kWh) const override
    { return rentalAndLate(daysUsed, daysBooked) + kWh * POWER_PRICE_PER_KWH + daysUsed * DRIVER_FEE_PER_DAY; }
    void print() const override
    {
        MotorVehicle::print();
        std::cout << "  seats " << seats_ << "  battery " << batteryKWh_ << " kWh";
    }
};
// ---------------------------------------------------------------------------------------

int main()
{
    std::cout << std::fixed << std::setprecision(2);
    const Date today(15, 11, 2026);

    Fleet fleet;
    fleet.add(new Bicycle("BIC-001", 150));
    fleet.add(new Car("TN30-5678", 1500, 60, Date(10, 12, 2026), 5));
    fleet.add(new ElectricBus("TN30-BUS1", 6000, 90, Date(5, 3, 2027), 40, 250.0));   // the new type

    std::cout << "FLEET (the bus is handled by the same Fleet code)\n";
    fleet.list(today);

    std::cout << "\nrent bus for 2 days: " << (fleet.rent("TN30-BUS1", 2) ? "OK" : "REFUSED") << '\n';
    double amount = fleet.giveBack("TN30-BUS1", 3, 120);        // 1 day late, 120 kWh
    std::cout << "bus returned after 3 days, 120 kWh -> Rs." << amount << '\n';

    std::cout << "\nFLEET AFTER RETURN\n";
    fleet.list(today);
    return 0;
}
