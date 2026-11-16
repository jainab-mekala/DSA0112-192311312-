// Task 1.2 - Repaired design: common abstract base class, private data, pay() overridden.
//
// UML CLASS DIAGRAM (draw this in your report):
//
//                    +----------------------------+
//                    |      <<abstract>> Staff    |
//                    +----------------------------+
//                    | - name_ : string           |
//                    | - id_   : int              |
//                    +----------------------------+
//                    | + name() : string          |
//                    | + id()   : int             |
//                    | + pay()  : double  {abstract}
//                    | + category() : string {abstract}
//                    +-------------^--------------+
//                       |                      |
//        +--------------+----------+     +-----+---------------+
//        |    SalariedStaff        |     |    GuestFaculty     |
//        +-------------------------+     +---------------------+
//        | - basic_ : double       |     | - hours_ : double   |
//        | # da() : double         |     | - rate_  : double   |
//        | + pay() : double        |     | + pay() : double    |
//        +------^-------------^----+     +---------------------+
//               |             |
//        +------+-----+  +----+-------------+     (new category added later:)
//        | Professor  |  |  LabAssistant    |     AdminOfficer : SalariedStaff
//        | - research_|  |                  |
//        | + pay()    |  | + pay()          |
//        +------------+  +------------------+
//
// Inheritance: multilevel (Staff -> SalariedStaff -> Professor) + hierarchical (several
// classes derived from Staff / SalariedStaff).
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

const double DA_RATE = 0.42;          // the magic number now lives in ONE named constant

class Staff {                          // abstract base: what every staff member has
    std::string name_;
    int         id_;
public:
    Staff(const std::string& name, int id) : name_(name), id_(id) {}
    virtual ~Staff() = default;
    const std::string& name() const { return name_; }
    int id() const { return id_; }
    virtual double pay() const = 0;
    virtual std::string category() const = 0;
};

class SalariedStaff : public Staff {   // staff paid basic + DA
    double basic_;
protected:
    double da() const { return basic_ * DA_RATE; }
public:
    SalariedStaff(const std::string& name, int id, double basic) : Staff(name, id), basic_(basic) {}
    double pay() const override { return basic_ + da(); }
};

class Professor : public SalariedStaff {
    double research_;
public:
    Professor(const std::string& name, int id, double basic, double research)
        : SalariedStaff(name, id, basic), research_(research) {}
    double pay() const override { return SalariedStaff::pay() + research_; }
    std::string category() const override { return "Professor"; }
};

class LabAssistant : public SalariedStaff {
public:
    LabAssistant(const std::string& name, int id, double basic) : SalariedStaff(name, id, basic) {}
    double pay() const override { return SalariedStaff::pay(); }
    std::string category() const override { return "Lab Assistant"; }
};

class GuestFaculty : public Staff {
    double hours_, rate_;
public:
    GuestFaculty(const std::string& name, int id, double hours, double rate)
        : Staff(name, id), hours_(hours), rate_(rate) {}
    double pay() const override { return hours_ * rate_; }
    std::string category() const override { return "Guest Faculty"; }
};

// ---- NEW CATEGORY: added without editing any class above -----------------------------
class AdminOfficer : public SalariedStaff {
    double allowance_;
public:
    AdminOfficer(const std::string& name, int id, double basic, double allowance)
        : SalariedStaff(name, id, basic), allowance_(allowance) {}
    double pay() const override { return SalariedStaff::pay() + allowance_; }
    std::string category() const override { return "Admin Officer"; }
};

int main()
{
    std::vector< std::unique_ptr<Staff> > all;       // ONE container for every kind of staff
    all.push_back(std::unique_ptr<Staff>(new Professor("Dr. Rao", 1, 80000, 5000)));
    all.push_back(std::unique_ptr<Staff>(new LabAssistant("Meena", 2, 30000)));
    all.push_back(std::unique_ptr<Staff>(new GuestFaculty("Mr. Khan", 3, 20, 1500)));
    all.push_back(std::unique_ptr<Staff>(new AdminOfficer("Priya", 4, 45000, 3000)));

    double total = 0;
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& s : all) {                       // ONE loop for all staff
        std::cout << std::left << std::setw(15) << s->category() << std::setw(10) << s->name()
                  << "id " << s->id() << "   pay " << s->pay() << '\n';
        total += s->pay();
    }
    std::cout << "Total payroll: " << total << '\n';
    return 0;
}
