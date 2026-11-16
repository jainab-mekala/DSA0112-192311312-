// Task 2 - the same program with the word 'virtual' REMOVED from both base classes.
// Predict the new output and the compiler messages first, then compile with -Wall -Wextra.
#include <iostream>

struct Person { Person() { std::cout << "Person "; } };
struct Student : Person { Student() { std::cout << "Student "; } };
struct Staff : Person { Staff() { std::cout << "Staff "; } };

struct TeachingAssistant : Student, Staff {
    TeachingAssistant() { std::cout << "TA "; }
};

int main()
{
    TeachingAssistant ta;
    std::cout << '\n';
}
