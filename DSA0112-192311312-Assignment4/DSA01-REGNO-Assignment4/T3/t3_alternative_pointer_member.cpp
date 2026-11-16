// Task 3 - Alternative concept: Student holds Address* (created with new) instead of an
// Address member object. Shows what the constructor, copy constructor, assignment and
// destructor must now do, and measures the extra allocation time and space.
#include <iostream>
#include <string>
#include <ctime>

class Address {
    std::string city_; int pin_;
public:
    static int live;                                     // how many Address objects exist now
    Address(const std::string& c, int p) : city_(c), pin_(p) { ++live; }
    Address(const Address& o) : city_(o.city_), pin_(o.pin_) { ++live; }
    ~Address() { --live; }
    void setCity(const std::string& c) { city_ = c; }
    std::string str() const { return city_ + "-" + std::to_string(pin_); }
};
int Address::live = 0;

class Person {
protected:
    std::string name; int age;
public:
    Person(const std::string& n, int a) : name(n), age(a) {}
};

// ---- (A) member object: compiler-generated copy/destructor are enough ----
class StudentObj : public Person {
    long long regNo; Address home;
public:
    StudentObj(const std::string& n, int a, long long r, const std::string& c, int p)
        : Person(n, a), regNo(r), home(c, p) {}
    void setCity(const std::string& c) { home.setCity(c); }
    std::string str() const { return name + " " + home.str(); }
    long long reg() const { return regNo; }
};

// ---- (B) pointer member: the Rule of Three becomes our job ----
class StudentPtr : public Person {
    long long regNo; Address* home;
public:
    StudentPtr(const std::string& n, int a, long long r, const std::string& c, int p)
        : Person(n, a), regNo(r), home(new Address(c, p)) {}          // constructor: allocate
    StudentPtr(const StudentPtr& o)                                   // copy constructor: DEEP copy
        : Person(o), regNo(o.regNo), home(new Address(*o.home)) {}
    StudentPtr& operator=(const StudentPtr& o)                        // assignment: deep copy, self-safe
    {
        if (this != &o) { Address* fresh = new Address(*o.home); delete home; home = fresh;
                          Person::operator=(o); regNo = o.regNo; }
        return *this;
    }
    ~StudentPtr() { delete home; }                                    // destructor: release
    void setCity(const std::string& c) { home->setCity(c); }
    std::string str() const { return name + " " + home->str(); }
    long long reg() const { return regNo; }
};
// If StudentPtr kept the compiler-generated copy constructor, two students would share ONE
// Address (shallow copy) and the second destructor would delete it again: double free / crash.

volatile long long sink = 0;

int main()
{
    std::cout << "sizeof(StudentObj) = " << sizeof(StudentObj) << "\n";
    std::cout << "sizeof(StudentPtr) = " << sizeof(StudentPtr) << "  (+ one separate heap block for Address)\n\n";

    {
        StudentPtr a("Kavya", 19, 1, "Salem", 636001);
        StudentPtr b = a;                       // deep copy
        b.setCity("Chennai");
        std::cout << "a: " << a.str() << "\nb: " << b.str() << "   (independent addresses: deep copy works)\n";
        std::cout << "live Address objects while a and b exist: " << Address::live << '\n';
    }
    std::cout << "live Address objects after both are destroyed: " << Address::live << "\n\n";

    const int N = 2000000;
    long long total = 0;
    clock_t t0 = clock();
    for (int i = 0; i < N; ++i) { StudentObj s("Kavya", 19, i, "Salem", 636001); total += s.reg(); }
    clock_t t1 = clock();
    for (int i = 0; i < N; ++i) { StudentPtr s("Kavya", 19, i, "Salem", 636001); total += s.reg(); }
    clock_t t2 = clock();
    sink = total;
    std::cout << N << " constructions + destructions\n";
    std::cout << "member object : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "pointer (new) : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    return 0;
}
