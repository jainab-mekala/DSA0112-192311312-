// Task 3 - "Swap the order of the items in your initializer list, compile with -Wall and
// record the warning." Same program as t3_complete_code.cpp, initializer list reordered.
// Expected: -Wreorder warnings. The REAL initialisation order is: base class first, then
// members in the order they are DECLARED in the class (regNo, then home) - NOT the order
// written in the initializer list.
#include <iostream>
#include <string>

class Person {
protected:
    std::string name;
    int age;
public:
    Person(const std::string& n, int a) : name(n), age(a) {}
};

class Address {
    std::string city;
    int pin;
public:
    Address(const std::string& c, int p) : city(c), pin(p) {}
    std::string str() const { return city + "-" + std::to_string(pin); }
};

class Student : public Person {
    long long regNo;
    Address home;
public:
    Student(const std::string& n, int a, long long r, const std::string& c, int p)
        : home(c, p), regNo(r), Person(n, a) {}          // written in the "wrong" order

    void print() const
    {
        std::cout << regNo << ' ' << name << " (" << age << ") " << home.str() << '\n';
    }
};

int main()
{
    Student s("Kavya", 19, 192311001, "Salem", 636001);
    s.print();
}
