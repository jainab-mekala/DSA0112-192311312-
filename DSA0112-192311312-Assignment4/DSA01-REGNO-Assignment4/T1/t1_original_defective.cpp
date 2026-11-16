// Task 1.1 - the ORIGINAL three classes (exactly as given) plus a small main that
// makes each deficiency visible. Use the output + the comments as evidence for the diagnosis.
#include <iostream>
#include <string>
using std::string;

class Professor { public: string name; int id; double basic; double da; double research;
                  double pay() { return basic + basic * 0.42 + research; } };

class LabAssistant { public: string name; int id; double basic; double da;
                     double pay() { return basic + basic * 0.42; } };

class GuestFaculty { public: string nm; int code; double hours; double rate;
                     double pay() { return hours * rate; } };

int main()
{
    Professor    p; p.name = "Dr. Rao";  p.id = 1;   p.basic = 80000; p.da = 0; p.research = 5000;
    LabAssistant l; l.name = "Meena";    l.id = 2;   l.basic = 30000; l.da = 0;
    GuestFaculty g; g.nm   = "Mr. Khan"; g.code = 3; g.hours = 20;    g.rate = 1500;

    // (1) three separate pay() calls: there is no common type, so no single loop is possible
    std::cout << p.name << " : " << p.pay() << '\n';
    std::cout << l.name << " : " << l.pay() << '\n';
    std::cout << g.nm   << " : " << g.pay() << '\n';
    // Professor staff[3] = {p, l, g};   // <-- impossible: different types, no common base

    // (2) public data members: anybody can corrupt the data, there is no validation
    g.hours = -500;
    std::cout << "GuestFaculty after g.hours = -500 : " << g.pay() << "   (negative pay accepted)\n";

    // (3) the 'da' field is never used by pay(): setting it has no effect
    l.da = 99999;
    std::cout << "LabAssistant after l.da = 99999   : " << l.pay() << "   (unchanged - da is dead data)\n";

    // (4) same information, different names: name/nm and id/code
    std::cout << "ids: " << p.id << ", " << l.id << ", " << g.code << "   (id vs code)\n";

    // (5) magic number 0.42 is hard-coded twice (Professor and LabAssistant)
    // (6) name, id, basic, da are repeated in two classes: a change must be made in each
    // (7) a new staff category = a new class + edits to every piece of code that processes staff
    std::cout << "sizeof: Professor=" << sizeof(Professor) << " LabAssistant=" << sizeof(LabAssistant)
              << " GuestFaculty=" << sizeof(GuestFaculty) << '\n';
    return 0;
}
