// A1.1 / A1.2 / A1.3 - Polymorphic bill hierarchy
//
// A1.1 COMMON INTERFACE: every bill provides total(), type(), print();
//      common data: patient name (and a package code used by government schemes later).
//
// A1.2 UML CLASS DIAGRAM  ( {pure} = pure virtual, {virtual} = virtual, {override} = overridden )
//
//            +-------------------------------------------+
//            |            <<abstract>> Bill              |
//            +-------------------------------------------+
//            | # patient_ : string                       |
//            +-------------------------------------------+
//            | + ~Bill()          {virtual}              |
//            | + total() : double {pure virtual}         |
//            | + type()  : string {pure virtual}         |
//            | + print() : void   {non-virtual, calls the two above} |
//            +--------^-------------^----------^--------^+
//                     |             |          |        |
//    +----------------+--+  +-------+--------+ |  +-----+--------------+
//    |   OutPatientBill  |  |  InPatientBill | |  |   EmergencyBill    |
//    | - consultation_   |  | - days_        | |  | - base_            |
//    | - tests_          |  | - perDay_      | |  | - hours_           |
//    | + total() {override}| | - procedures_ | |  | - hourlyRate_      |
//    | + type()  {override}| | - medicines_  | |  | + total() {override}|
//    +-------------------+  | + total() {override} | + type()  {override}|
//                           | + type()  {override} +--------------------+
//                           +----------------+  +--------------------+
//                                               |   DayCareBill      |
//                                               | - packagePrice_    |
//                                               | + total() {override}|
//                                               | + type()  {override}|
//                                               +--------------------+
//   (all four derived classes inherit publicly from Bill: hierarchical inheritance)
//
// A1.3 PSEUDOCODE - day's total for an array of base-class pointers
//   ALGORITHM DayTotal(bills[0..n-1])
//   INPUT : bills - array of n pointers to Bill (each may point to ANY kind of bill)
//   OUTPUT: sum   - total of all bills of the day
//   BEGIN
//       sum <- 0
//       FOR i <- 0 TO n - 1 DO
//           sum <- sum + bills[i]->total()        // late binding picks the right total()
//       END FOR
//       RETURN sum
//   END
#include <iostream>
#include <iomanip>
#include <string>

class Bill {
protected:
    std::string patient_;
public:
    explicit Bill(const std::string& patient) : patient_(patient) {}
    virtual ~Bill() = default;
    virtual double total() const = 0;
    virtual std::string type() const = 0;
    void print() const
    {
        std::cout << std::left << std::setw(4) << type() << std::setw(10) << patient_
                  << std::right << std::setw(10) << total() << '\n';
    }
};

class OutPatientBill : public Bill {
    double consultation_, tests_;
public:
    OutPatientBill(const std::string& p, double consultation, double tests)
        : Bill(p), consultation_(consultation), tests_(tests) {}
    double total() const override { return consultation_ + tests_; }
    std::string type() const override { return "OP"; }
};

class InPatientBill : public Bill {
    int days_; double perDay_, procedures_, medicines_;
public:
    InPatientBill(const std::string& p, int days, double perDay, double procedures, double medicines)
        : Bill(p), days_(days), perDay_(perDay), procedures_(procedures), medicines_(medicines) {}
    double total() const override { return days_ * perDay_ + procedures_ + medicines_; }
    std::string type() const override { return "IP"; }
};

class DayCareBill : public Bill {
    double packagePrice_;
public:
    DayCareBill(const std::string& p, double packagePrice) : Bill(p), packagePrice_(packagePrice) {}
    double total() const override { return packagePrice_; }
    std::string type() const override { return "DC"; }
};

class EmergencyBill : public Bill {
    double base_, hours_, hourlyRate_;
public:
    EmergencyBill(const std::string& p, double base, double hours, double hourlyRate)
        : Bill(p), base_(base), hours_(hours), hourlyRate_(hourlyRate) {}
    double total() const override { return base_ + hours_ * hourlyRate_; }
    std::string type() const override { return "ER"; }
};

double dayTotal(Bill* const bills[], int n)               // works for every kind of bill
{
    double sum = 0;
    for (int i = 0; i < n; ++i) sum += bills[i]->total();
    return sum;
}

int main()
{
    Bill* bills[] = {
        new OutPatientBill("Ravi", 500, 1200),
        new InPatientBill("Meena", 3, 2500, 40000, 5000),
        new DayCareBill("Kiran", 25000),
        new EmergencyBill("Sara", 3000, 2.5, 800)
    };
    const int n = sizeof(bills) / sizeof(bills[0]);

    for (int i = 0; i < n; ++i) bills[i]->print();
    std::cout << "Day's total: " << dayTotal(bills, n) << '\n';

    for (int i = 0; i < n; ++i) delete bills[i];           // virtual destructor: correct destructor runs
    return 0;
}
