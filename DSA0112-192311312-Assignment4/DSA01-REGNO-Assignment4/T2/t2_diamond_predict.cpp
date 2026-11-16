// Task 2 - Predict the Output (program exactly as given). Predict BEFORE running.
#include <iostream>

struct Person { Person() { std::cout << "Person "; } };
struct Student : virtual Person { Student() { std::cout << "Student "; } };
struct Staff : virtual Person { Staff() { std::cout << "Staff "; } };

struct TeachingAssistant : Student, Staff {
    TeachingAssistant() { std::cout << "TA "; }
};

int main()
{
    TeachingAssistant ta;
    std::cout << '\n';
}
