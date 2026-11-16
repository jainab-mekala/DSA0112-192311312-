// Task 6 - Alternative concept: ONE Vehicle class with a type code and a switch in charge().
// Compares space per object, time per charge() call (switch vs virtual call) and the
// number of places that change when a new vehicle type (ElectricBus) is added.
#include "t6_vehicle_fleet.h"
#include <vector>
#include <ctime>

// ---------------- single fat class ----------------
class FatVehicle {
public:
    enum Type { BICYCLE, TWO_WHEELER, CAR, ELECTRIC_CAR };      // new type -> edit #1: add enumerator
private:
    Type        type_;
    std::string regNo_;
    double      dailyRate_;
    bool        rented_;
    int         daysBooked_;
    double      level_;          // unused by BICYCLE
    int         serviceDue_;     // unused by BICYCLE
    int         seats_;          // used by CAR / ELECTRIC_CAR only
    double      batteryKWh_;     // used by ELECTRIC_CAR only
public:
    FatVehicle(Type t, const std::string& reg, double rate, double level = 0, int due = 0,
               int seats = 0, double battery = 0)               // new type -> edit #2: constructor
        : type_(t), regNo_(reg), dailyRate_(rate), rented_(false), daysBooked_(0),
          level_(level), serviceDue_(due), seats_(seats), batteryKWh_(battery) {}

    double charge(int daysUsed, int daysBooked, double energy) const
    {
        int late = daysUsed > daysBooked ? daysUsed - daysBooked : 0;
        double base = daysUsed * dailyRate_ + late * dailyRate_ * LATE_SURCHARGE;
        switch (type_) {                                         // new type -> edit #3: add a case
            case BICYCLE:      return base;
            case TWO_WHEELER:  return base + energy * PETROL_PRICE_PER_LITRE;
            case CAR:          return base + energy * PETROL_PRICE_PER_LITRE + CLEANING_FEE;
            case ELECTRIC_CAR: return base + energy * POWER_PRICE_PER_KWH + CLEANING_FEE;
        }
        return base;
    }
    // print(), serviceOverdue(), rent() ... would each need their own switch too  -> edits #4, #5, ...
};

volatile double sink = 0;

int main()
{
    std::cout << "SPACE PER OBJECT (bytes)\n";
    std::cout << "  FatVehicle (every type)  : " << sizeof(FatVehicle) << '\n';
    std::cout << "  Bicycle                  : " << sizeof(Bicycle) << '\n';
    std::cout << "  TwoWheeler               : " << sizeof(TwoWheeler) << '\n';
    std::cout << "  Car                      : " << sizeof(Car) << '\n';
    std::cout << "  ElectricCar              : " << sizeof(ElectricCar) << "   (each includes one vtable pointer)\n\n";

    const int N = 1000, ROUNDS = 20000;                          // 20,000,000 charge() calls each
    std::vector<FatVehicle> fat;
    std::vector<Vehicle*> poly;
    for (int i = 0; i < N; ++i) {
        std::string reg = "V" + std::to_string(i);
        switch (i % 4) {
            case 0: fat.push_back(FatVehicle(FatVehicle::BICYCLE, reg, 150));
                    poly.push_back(new Bicycle(reg, 150)); break;
            case 1: fat.push_back(FatVehicle(FatVehicle::TWO_WHEELER, reg, 400, 70, 1));
                    poly.push_back(new TwoWheeler(reg, 400, 70, Date(1, 1, 2027))); break;
            case 2: fat.push_back(FatVehicle(FatVehicle::CAR, reg, 1500, 60, 1, 5));
                    poly.push_back(new Car(reg, 1500, 60, Date(1, 1, 2027), 5)); break;
            default: fat.push_back(FatVehicle(FatVehicle::ELECTRIC_CAR, reg, 2200, 80, 1, 5, 40));
                    poly.push_back(new ElectricCar(reg, 2200, 80, Date(1, 1, 2027), 5, 40)); break;
        }
    }

    double a = 0, b = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) a += fat[i].charge(r % 7 + 1, 3, 5.0);
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) b += poly[i]->charge(r % 7 + 1, 3, 5.0);
    clock_t t2 = clock();
    sink = a + b;

    std::cout << "TIME for " << N * ROUNDS << " charge() calls\n";
    std::cout << "  switch in one class : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "  virtual call        : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "  same totals?          " << (a == b ? "yes" : "NO") << " (" << a << ")\n";

    for (int i = 0; i < N; ++i) delete poly[i];
    return 0;
}
