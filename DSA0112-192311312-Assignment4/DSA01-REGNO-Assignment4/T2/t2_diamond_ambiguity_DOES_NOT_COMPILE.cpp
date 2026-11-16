// Task 2.2 - THE DIAMOND PROBLEM. THIS FILE IS MEANT TO FAIL TO COMPILE.
// Without virtual bases a TeachingAssistant contains TWO Person sub-objects, so
// ta.name / ta.id are ambiguous. Screenshot the error as evidence.
// Fix 1: make both inheritances 'virtual' (one shared Person).
// Fix 2: qualify every use, e.g. ta.Student::id (but there are still two different ids!).
#include <iostream>
#include <string>

struct Person { std::string name; int id; Person() : name("unknown"), id(0) {} };
struct Student : Person { double cgpa; };
struct Staff : Person { double salary; };
struct TeachingAssistant : Student, Staff {};

int main()
{
    TeachingAssistant ta;
    ta.Student::id = 301;                 // allowed, but sets only the Student's copy of Person
    std::cout << ta.id << '\n';           // ERROR: request for member 'id' is ambiguous
    std::cout << ta.name << '\n';         // ERROR: request for member 'name' is ambiguous
    return 0;
}
