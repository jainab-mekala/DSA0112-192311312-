// Task 2.1 / 2.3 - University people hierarchy (class declarations with access specifiers)
//
// RELATIONSHIPS
//   Student  is-a Person            (single inheritance, public, virtual base)
//   Staff    is-a Person            (single inheritance, public, virtual base)
//   TeachingAssistant is-a Student AND is-a Staff   -> MULTIPLE inheritance
//   ResearchScholar is-a Student                    -> single inheritance (multilevel: Person->Student->ResearchScholar)
//   Person -> Student, Person -> Staff              -> hierarchical;  whole structure = HYBRID (diamond)
//   Person has-a Address / Student has-a Course     -> member classes (see Task 3)
//
//                Person
//               /      \  (virtual)
//          Student    Staff
//          /     \     /
//  ResearchScholar \   /
//               TeachingAssistant
#include <iostream>
#include <string>

class Person {
    std::string name_;
    int         id_;
    std::string phone_;
public:
    Person(const std::string& name, int id, const std::string& phone)
        : name_(name), id_(id), phone_(phone) {}
    virtual ~Person() = default;
    const std::string& name() const  { return name_; }
    int id() const                   { return id_; }
    const std::string& phone() const { return phone_; }
    virtual void print() const
    {
        std::cout << "[" << id_ << "] " << name_ << ", phone " << phone_;
    }
};

class Student : public virtual Person {            // public: a Student IS-A Person for everyone
    std::string programme_;
    double      cgpa_;
public:
    Student(const std::string& name, int id, const std::string& phone,
            const std::string& programme, double cgpa)
        : Person(name, id, phone), programme_(programme), cgpa_(cgpa) {}
    void printStudentPart() const { std::cout << " | " << programme_ << ", CGPA " << cgpa_; }
    void print() const override   { Person::print(); printStudentPart(); }
};

class Staff : public virtual Person {
    std::string department_;
    double      salary_;
public:
    Staff(const std::string& name, int id, const std::string& phone,
          const std::string& department, double salary)
        : Person(name, id, phone), department_(department), salary_(salary) {}
    void printStaffPart() const { std::cout << " | " << department_ << ", salary " << salary_; }
    void print() const override { Person::print(); printStaffPart(); }
};

class TeachingAssistant : public Student, public Staff {
public:
    TeachingAssistant(const std::string& name, int id, const std::string& phone,
                      const std::string& programme, double cgpa,
                      const std::string& department, double salary)
        : Person(name, id, phone),                       // most-derived class builds the virtual base
          Student(name, id, phone, programme, cgpa),
          Staff(name, id, phone, department, salary) {}
    // Student::print and Staff::print both override Person::print, so TA MUST override it too
    // (otherwise the call is ambiguous and the program does not compile).
    void print() const override
    {
        Person::print();
        printStudentPart();
        printStaffPart();
    }
};

class ResearchScholar : public Student {
    double fellowship_;
public:
    ResearchScholar(const std::string& name, int id, const std::string& phone,
                    const std::string& programme, double cgpa, double fellowship)
        : Person(name, id, phone), Student(name, id, phone, programme, cgpa), fellowship_(fellowship) {}
    void print() const override { Student::print(); std::cout << " | fellowship " << fellowship_; }
};

int main()
{
    Student           s("Kavya", 101, "9000000001", "B.Tech CSE", 8.7);
    Staff             f("Dr. Ravi", 201, "9000000002", "CSE", 90000);
    TeachingAssistant ta("Anu", 301, "9000000003", "M.Tech AI", 9.1, "CSE", 25000);
    ResearchScholar   rs("Vikram", 401, "9000000004", "Ph.D. CSE", 9.4, 31000);

    const Person* all[4] = { &s, &f, &ta, &rs };        // every kind through a Person pointer
    for (int i = 0; i < 4; ++i) { all[i]->print(); std::cout << '\n'; }

    std::cout << "\nta.name() is unambiguous thanks to the virtual base: " << ta.name() << '\n';
    std::cout << "sizeof(TeachingAssistant) = " << sizeof(TeachingAssistant) << " bytes\n";
    return 0;
}
