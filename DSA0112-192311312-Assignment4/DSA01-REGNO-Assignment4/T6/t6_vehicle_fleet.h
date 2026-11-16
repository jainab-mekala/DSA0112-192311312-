// ============================================================================
// Task 6 - Vehicle Rental Fleet : the class hierarchy (header only)
// Used by t6_fleet_demo.cpp, t6_extension_electric_bus.cpp and t6_alternative_single_class_switch.cpp
// Put all .h/.cpp files of this folder in the same directory when you compile.
//
// STAGE 1 - REQUIREMENTS (testable statements)
//   R1  Every vehicle has a registration number and a daily rate.
//   R2  A motor vehicle also has a fuel/charge level and a service due date.
//   R3  A car has a seating capacity; an electric car also has a battery capacity (kWh).
//   R4  A vehicle can be rented for a number of days, only if it is not already rented.
//   R5  A vehicle can be returned; the charge is computed by the vehicle's OWN rule:
//         bicycle      = days*rate + late surcharge
//         two-wheeler  = days*rate + late surcharge + litres*petrol price
//         car          = days*rate + late surcharge + litres*petrol price + cleaning fee
//         electric car = days*rate + late surcharge + kWh*power price     + cleaning fee
//       late surcharge = 50% of the daily rate for each day beyond the booked days.
//   R6  All vehicles can be listed with one loop; overdue service is flagged.
//   R7  A new vehicle type can be added WITHOUT editing any existing class.
//
// STAGE 2 - HIERARCHY (UML)
//
//                       +------------------------------+
//                       |   <<abstract>> Vehicle       |
//                       | - regNo_, dailyRate_         |
//                       | - rented_, daysBooked_       |
//                       | + rent(days), giveBack(...)  |
//                       | + charge(...) = 0   (pure)   |
//                       | + type()      = 0   (pure)   |
//                       | + print(), serviceOverdue()  |
//                       +--------^---------------^-----+
//                                |               |
//                      +---------+----+     +----+-------------------+
//                      |  Bicycle     |     |  MotorVehicle          |
//                      +--------------+     | - level_  ◆-- Date serviceDue_ (member class)
//                                           +-----^-------------^----+
//                                                 |             |
//                                          +------+-----+  +----+-----+
//                                          | TwoWheeler |  |   Car    | - seats_
//                                          +------------+  +----^-----+
//                                                               |
//                                                        +------+-------+
//                                                        | ElectricCar  | - batteryKWh_
//                                                        +--------------+
//   Inheritance types: single (Bicycle), multilevel (Vehicle -> MotorVehicle -> Car -> ElectricCar),
//   hierarchical (Bicycle and MotorVehicle both derive from Vehicle). 'Date' is a member class (has-a).
//   The fleet is an ARRAY OF Vehicle* (class Fleet).
//
// STAGE 3 - KEY DECISIONS
//   * common data (regNo, rate, rented state) lives once in Vehicle (private; protected helper for charges)
//   * virtual: charge(), type(), print(), serviceOverdue(), destructor
//   * charge() is PURE virtual -> Vehicle is abstract, every concrete type MUST define its rule
//   * Fleet stores Vehicle* and owns them; copying Vehicle/Fleet is forbidden (no slicing, no double delete)
//
// STAGE 4 - PSEUDOCODE
//   RENT(vehicle, days):
//       IF vehicle.rented OR days <= 0 THEN RETURN failure
//       vehicle.rented <- true
//       vehicle.daysBooked <- days
//       RETURN success
//
//   RETURN(vehicle, daysUsed, energyUsed):
//       IF NOT vehicle.rented THEN RETURN failure
//       late <- MAX(0, daysUsed - vehicle.daysBooked)
//       amount <- vehicle.charge(daysUsed, vehicle.daysBooked, energyUsed)   // virtual: type-specific rule
//       vehicle.rented <- false
//       vehicle.daysBooked <- 0
//       RETURN amount
// ============================================================================
#ifndef T6_VEHICLE_FLEET_H
#define T6_VEHICLE_FLEET_H

#include <iostream>
#include <iomanip>
#include <string>

const double PETROL_PRICE_PER_LITRE = 100.0;
const double POWER_PRICE_PER_KWH    = 12.0;
const double LATE_SURCHARGE         = 0.5;      // extra 50% of the daily rate per late day
const double CLEANING_FEE           = 200.0;

class Date {                                     // member class: service due date
    int d_, m_, y_;
public:
    Date(int d, int m, int y) : d_(d), m_(m), y_(y) {}
    int serial() const { return y_ * 372 + m_ * 31 + d_; }          // good enough for ordering
    bool isBefore(const Date& o) const { return serial() < o.serial(); }
    std::string str() const { return std::to_string(d_) + "/" + std::to_string(m_) + "/" + std::to_string(y_); }
};

class Vehicle {                                  // abstract base class
    std::string regNo_;
    double      dailyRate_;
    bool        rented_;
    int         daysBooked_;
protected:
    double dailyRate() const { return dailyRate_; }
    double rentalAndLate(int daysUsed, int daysBooked) const
    {
        int late = daysUsed > daysBooked ? daysUsed - daysBooked : 0;
        return daysUsed * dailyRate_ + late * dailyRate_ * LATE_SURCHARGE;
    }
public:
    Vehicle(const std::string& regNo, double dailyRate)
        : regNo_(regNo), dailyRate_(dailyRate), rented_(false), daysBooked_(0) {}
    Vehicle(const Vehicle&) = delete;            // polymorphic objects are never copied
    Vehicle& operator=(const Vehicle&) = delete;
    virtual ~Vehicle() = default;

    const std::string& regNo() const { return regNo_; }
    bool isRented() const { return rented_; }

    virtual std::string type() const = 0;                                           // pure virtual
    virtual double charge(int daysUsed, int daysBooked, double energyUsed) const = 0; // pure virtual
    virtual bool serviceOverdue(const Date&) const { return false; }
    virtual void print() const
    {
        std::cout << std::left << std::setw(13) << type() << std::setw(11) << regNo_
                  << "Rs." << std::setw(9) << dailyRate_ << (rented_ ? "RENTED   " : "available");
    }

    bool rent(int days)
    {
        if (rented_ || days <= 0) return false;
        rented_ = true;
        daysBooked_ = days;
        return true;
    }
    double giveBack(int daysUsed, double energyUsed)                                  // -1 means "was not rented"
    {
        if (!rented_) return -1;
        double amount = charge(daysUsed, daysBooked_, energyUsed);
        rented_ = false;
        daysBooked_ = 0;
        return amount;
    }
};

class Bicycle : public Vehicle {
public:
    Bicycle(const std::string& regNo, double dailyRate) : Vehicle(regNo, dailyRate) {}
    std::string type() const override { return "Bicycle"; }
    double charge(int daysUsed, int daysBooked, double) const override
    { return rentalAndLate(daysUsed, daysBooked); }
};

class MotorVehicle : public Vehicle {
    double level_;                               // fuel litres% or battery charge%
    Date   serviceDue_;                          // member class
public:
    MotorVehicle(const std::string& regNo, double dailyRate, double level, const Date& serviceDue)
        : Vehicle(regNo, dailyRate), level_(level), serviceDue_(serviceDue) {}
    double level() const { return level_; }
    bool serviceOverdue(const Date& today) const override { return serviceDue_.isBefore(today); }
    void print() const override
    {
        Vehicle::print();
        std::cout << "  level " << level_ << "%  service due " << serviceDue_.str();
    }
};

class TwoWheeler : public MotorVehicle {
public:
    TwoWheeler(const std::string& regNo, double dailyRate, double level, const Date& serviceDue)
        : MotorVehicle(regNo, dailyRate, level, serviceDue) {}
    std::string type() const override { return "Two-wheeler"; }
    double charge(int daysUsed, int daysBooked, double litres) const override
    { return rentalAndLate(daysUsed, daysBooked) + litres * PETROL_PRICE_PER_LITRE; }
};

class Car : public MotorVehicle {
    int seats_;
public:
    Car(const std::string& regNo, double dailyRate, double level, const Date& serviceDue, int seats)
        : MotorVehicle(regNo, dailyRate, level, serviceDue), seats_(seats) {}
    int seats() const { return seats_; }
    std::string type() const override { return "Car"; }
    double charge(int daysUsed, int daysBooked, double litres) const override
    { return rentalAndLate(daysUsed, daysBooked) + litres * PETROL_PRICE_PER_LITRE + CLEANING_FEE; }
    void print() const override { MotorVehicle::print(); std::cout << "  seats " << seats_; }
};

class ElectricCar : public Car {
    double batteryKWh_;
public:
    ElectricCar(const std::string& regNo, double dailyRate, double level, const Date& serviceDue,
                int seats, double batteryKWh)
        : Car(regNo, dailyRate, level, serviceDue, seats), batteryKWh_(batteryKWh) {}
    std::string type() const override { return "Electric car"; }
    double charge(int daysUsed, int daysBooked, double kWh) const override       // charged per kWh, not per litre
    { return rentalAndLate(daysUsed, daysBooked) + kWh * POWER_PRICE_PER_KWH + CLEANING_FEE; }
    void print() const override { Car::print(); std::cout << "  battery " << batteryKWh_ << " kWh"; }
};

class Fleet {                                    // array of base-class pointers; the fleet OWNS the vehicles
    enum { MAX = 50 };
    Vehicle* v_[MAX];
    int      n_;
public:
    Fleet() : n_(0) {}
    Fleet(const Fleet&) = delete;
    Fleet& operator=(const Fleet&) = delete;
    ~Fleet() { for (int i = 0; i < n_; ++i) delete v_[i]; }       // virtual destructor makes this correct

    bool add(Vehicle* v)                         // takes ownership on success
    {
        if (n_ >= MAX) return false;
        v_[n_++] = v;
        return true;
    }
    Vehicle* find(const std::string& regNo) const
    {
        for (int i = 0; i < n_; ++i) if (v_[i]->regNo() == regNo) return v_[i];
        return nullptr;
    }
    bool rent(const std::string& regNo, int days)
    {
        Vehicle* v = find(regNo);
        return v && v->rent(days);
    }
    double giveBack(const std::string& regNo, int daysUsed, double energyUsed)
    {
        Vehicle* v = find(regNo);
        return v ? v->giveBack(daysUsed, energyUsed) : -1;
    }
    void list(const Date& today) const
    {
        for (int i = 0; i < n_; ++i) {
            v_[i]->print();                                       // polymorphic call
            if (v_[i]->serviceOverdue(today)) std::cout << "  ** SERVICE OVERDUE **";
            std::cout << '\n';
        }
    }
    int size() const { return n_; }
    const Vehicle* at(int i) const { return v_[i]; }
};

#endif
