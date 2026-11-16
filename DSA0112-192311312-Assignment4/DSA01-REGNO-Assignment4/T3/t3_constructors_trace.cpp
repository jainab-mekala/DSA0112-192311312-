// Task 3.1 / 3.2 / 3.3 - Constructors in derived classes, member classes, construction trace.
//  * Person has-a Address (member class), Student has-a Course (member class)
//  * Student, Staff and TeachingAssistant pass values to their bases with initializer lists
//  * Every constructor and destructor prints a message so the order can be recorded
#include <iostream>
#include <string>

class Address {                                          // member class of Person
    std::string city_; int pin_;
public:
    Address(const std::string& city, int pin) : city_(city), pin_(pin)
    { std::cout << "  Address(" << city_ << ") constructed\n"; }
    ~Address() { std::cout << "  Address(" << city_ << ") destroyed\n"; }
    std::string str() const { return city_ + "-" + std::to_string(pin_); }
};

class Course {                                           // member class of Student
    std::string code_, title_;
public:
    Course(const std::string& code, const std::string& title) : code_(code), title_(title)
    { std::cout << "  Course(" << code_ << ") constructed\n"; }
    ~Course() { std::cout << "  Course(" << code_ << ") destroyed\n"; }
    std::string str() const { return code_ + " " + title_; }
};

class Person {
    std::string name_; int id_; std::string phone_;
    Address     home_;                                   // has-a
public:
    Person(const std::string& name, int id, const std::string& phone,
           const std::string& city, int pin)
        : name_(name), id_(id), phone_(phone), home_(city, pin)
    { std::cout << "  Person(" << name_ << ") constructed\n"; }
    virtual ~Person() { std::cout << "  Person(" << name_ << ") destroyed\n"; }
    const std::string& name() const { return name_; }
    std::string address() const { return home_.str(); }
};

class Student : public virtual Person {
    std::string programme_; double cgpa_;
    Course      course_;                                 // has-a
public:
    Student(const std::string& name, int id, const std::string& phone,
            const std::string& city, int pin,
            const std::string& programme, double cgpa,
            const std::string& ccode, const std::string& ctitle)
        : Person(name, id, phone, city, pin),            // ignored when Student is a base of a TA!
          programme_(programme), cgpa_(cgpa), course_(ccode, ctitle)
    { std::cout << "  Student constructed\n"; }
    ~Student() override { std::cout << "  Student destroyed\n"; }
    std::string course() const { return course_.str(); }
};

class Staff : public virtual Person {
    std::string department_; double salary_;
public:
    Staff(const std::string& name, int id, const std::string& phone,
          const std::string& city, int pin,
          const std::string& department, double salary)
        : Person(name, id, phone, city, pin),            // ignored when Staff is a base of a TA!
          department_(department), salary_(salary)
    { std::cout << "  Staff constructed\n"; }
    ~Staff() override { std::cout << "  Staff destroyed\n"; }
};

class TeachingAssistant : public Student, public Staff {
public:
    TeachingAssistant(const std::string& name, int id, const std::string& phone,
                      const std::string& city, int pin,
                      const std::string& programme, double cgpa,
                      const std::string& ccode, const std::string& ctitle,
                      const std::string& department, double salary)
        : Person(name, id, phone, city, pin),            // the MOST-DERIVED class initialises the virtual base
          Student("IGNORED", 0, "0", "IGNORED", 0, programme, cgpa, ccode, ctitle),
          Staff("IGNORED", 0, "0", "IGNORED", 0, department, salary)
    { std::cout << "  TeachingAssistant constructed\n"; }
    ~TeachingAssistant() override { std::cout << "  TeachingAssistant destroyed\n"; }
};

int main()
{
    std::cout << "=== Student ===\n";
    {
        Student s("Kavya", 101, "9000000001", "Salem", 636001, "B.Tech CSE", 8.7, "CS101", "OOP with C++");
        std::cout << "  -> " << s.name() << ", " << s.address() << ", " << s.course() << '\n';
    }
    std::cout << "\n=== Staff ===\n";
    {
        Staff f("Dr. Ravi", 201, "9000000002", "Chennai", 600001, "CSE", 90000);
    }
    std::cout << "\n=== TeachingAssistant ===\n";
    {
        TeachingAssistant ta("Anu", 301, "9000000003", "Madurai", 625001, "M.Tech AI", 9.1,
                             "CS501", "Machine Learning", "CSE", 25000);
        std::cout << "  -> name is '" << ta.name() << "', address " << ta.address()
                  << "  (the \"IGNORED\" values passed through Student/Staff were never used)\n";
    }
    return 0;
}
