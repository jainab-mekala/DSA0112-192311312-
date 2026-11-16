// Task 2 - Alternative concept: TeachingAssistant HAS-A Student and HAS-A Staff (composition)
// versus inheriting from both. Compares sizeof, name-access time and ambiguity.
#include <iostream>
#include <string>
#include <ctime>

#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

struct Person { std::string name; int id; std::string phone; };

// (A) multiple inheritance with VIRTUAL bases : one Person
struct StudentV : virtual Person { std::string programme; double cgpa; };
struct StaffV   : virtual Person { std::string department; double salary; };
struct TA_Virtual : StudentV, StaffV {};

// (B) multiple inheritance WITHOUT virtual : two Persons, ambiguous access
struct StudentN : Person { std::string programme; double cgpa; };
struct StaffN   : Person { std::string department; double salary; };
struct TA_NonVirtual : StudentN, StaffN {};

// (C) composition : a TA HAS-A Student and HAS-A Staff (each contains its own Person)
struct TA_Composed { StudentN student; StaffN staff; };

NOINLINE int idVirtual(const TA_Virtual& t)     { return t.id; }                 // unambiguous, via virtual base
NOINLINE int idNonVirtual(const TA_NonVirtual& t) { return t.StudentN::id; }     // must qualify: WHICH id?
NOINLINE int idComposed(const TA_Composed& t)   { return t.student.id; }         // must choose a member: WHICH id?

volatile long long sink = 0;

int main()
{
    std::cout << "sizeof(Person)         = " << sizeof(Person) << '\n';
    std::cout << "sizeof(TA_Virtual)     = " << sizeof(TA_Virtual)     << "   (ONE Person + virtual-base pointers)\n";
    std::cout << "sizeof(TA_NonVirtual)  = " << sizeof(TA_NonVirtual)  << "   (TWO Person sub-objects)\n";
    std::cout << "sizeof(TA_Composed)    = " << sizeof(TA_Composed)    << "   (TWO Person sub-objects)\n\n";

    const int N = 1000, ROUNDS = 50000;                       // 50,000,000 accesses each
    static TA_Virtual v[N]; static TA_NonVirtual n[N]; static TA_Composed c[N];
    for (int i = 0; i < N; ++i) { v[i].id = i; n[i].StudentN::id = i; c[i].student.id = i; }

    long long a = 0, b = 0, d = 0;
    clock_t t0 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) a += idVirtual(v[i]);
    clock_t t1 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) b += idNonVirtual(n[i]);
    clock_t t2 = clock();
    for (int r = 0; r < ROUNDS; ++r) for (int i = 0; i < N; ++i) d += idComposed(c[i]);
    clock_t t3 = clock();
    sink = a + b + d;

    std::cout << "id via virtual base : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "id via qualified    : " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "id via member       : " << 1000.0 * (t3 - t2) / CLOCKS_PER_SEC << " ms\n";
    std::cout << "(checksums " << a << ", " << b << ", " << d << ")\n";
    return 0;
}
