// Task 3.4 - Experiment: remove 'virtual' from the base classes.
// Run it twice: once with USE_VIRTUAL 1, once with USE_VIRTUAL 0.
// Record (a) the construction/destruction order and (b) sizeof(TeachingAssistant).
#include <iostream>
#include <string>

#define USE_VIRTUAL 1          // <-- change to 0 for the experiment

#if USE_VIRTUAL
#define VBASE virtual
#else
#define VBASE
#endif

struct Person {
    std::string name; int id;
    Person() : name("p"), id(0) { std::cout << "Person "; }
    ~Person() { std::cout << "~Person "; }
};
struct Student : VBASE Person {
    std::string programme; double cgpa;
    Student() : cgpa(0) { std::cout << "Student "; }
    ~Student() { std::cout << "~Student "; }
};
struct Staff : VBASE Person {
    std::string department; double salary;
    Staff() : salary(0) { std::cout << "Staff "; }
    ~Staff() { std::cout << "~Staff "; }
};
struct TeachingAssistant : Student, Staff {
    TeachingAssistant() { std::cout << "TA "; }
    ~TeachingAssistant() { std::cout << "~TA "; }
};

int main()
{
    std::cout << "USE_VIRTUAL = " << USE_VIRTUAL << '\n';
    {
        TeachingAssistant ta;
        std::cout << "| ";
    }
    std::cout << '\n';
    std::cout << "sizeof(Person)            = " << sizeof(Person) << '\n';
    std::cout << "sizeof(Student)           = " << sizeof(Student) << '\n';
    std::cout << "sizeof(TeachingAssistant) = " << sizeof(TeachingAssistant) << '\n';
    return 0;
}
