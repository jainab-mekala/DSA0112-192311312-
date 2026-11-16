// Task 2.4 - Access table: how public / protected / private members of Person are
// visible in Student under public, protected and private inheritance.
//
//  Member in Person | public inheritance  | protected inheritance | private inheritance
//  -----------------+---------------------+-----------------------+---------------------
//  public           | public in Student   | protected in Student  | private in Student
//  protected        | protected in Student| protected in Student  | private in Student
//  private          | not accessible      | not accessible        | not accessible
//
// This program uses only the legal accesses. The commented lines are the illegal ones:
// uncomment them one at a time and screenshot the compiler errors as evidence.
#include <iostream>

class Person {
public:     int pub  = 1;
protected:  int prot = 2;
private:    int priv = 3;
public:     int readPriv() const { return priv; }     // Person's own function may read it
};

class StudentPublic : public Person {
public:
    void test() const
    {
        std::cout << "StudentPublic   inside : pub=" << pub << " prot=" << prot << '\n';
        // std::cout << priv;                 // ERROR: 'int Person::priv' is private
    }
};
class StudentProtected : protected Person {
public:
    void test() const
    {
        std::cout << "StudentProtected inside: pub=" << pub << " prot=" << prot << '\n';
    }
};
class StudentPrivate : private Person {
public:
    void test() const
    {
        std::cout << "StudentPrivate  inside : pub=" << pub << " prot=" << prot << '\n';
    }
};

// a class derived FURTHER from each Student shows what the inheritance mode really controls
class GradFromPublic    : public StudentPublic    { public: void test() const { std::cout << "GradFromPublic    : pub=" << pub << " prot=" << prot << '\n'; } };
class GradFromProtected : public StudentProtected { public: void test() const { std::cout << "GradFromProtected : pub=" << pub << " prot=" << prot << '\n'; } };
class GradFromPrivate   : public StudentPrivate   { public: void test() const { /* pub and prot are PRIVATE in StudentPrivate, so: */
    // std::cout << pub;                      // ERROR: 'int Person::pub' is inaccessible within this context
    std::cout << "GradFromPrivate   : (cannot touch any Person member)\n"; } };

int main()
{
    StudentPublic sp; StudentProtected sq; StudentPrivate sv;
    sp.test(); sq.test(); sv.test();

    std::cout << "outside, public inheritance: sp.pub = " << sp.pub << '\n';     // OK only for public inheritance
    // std::cout << sq.pub;                   // ERROR: 'int Person::pub' is inaccessible (protected inheritance)
    // std::cout << sv.pub;                   // ERROR: ... (private inheritance)
    // std::cout << sp.prot;                  // ERROR: 'int Person::prot' is protected within this context
    // Person* p = &sq;                       // ERROR: 'Person' is an inaccessible base of 'StudentProtected'
    Person* p = &sp;                          // OK only for public inheritance (is-a visible to everyone)
    std::cout << "Person* from StudentPublic works: pub = " << p->pub << ", readPriv() = " << p->readPriv() << "\n\n";

    GradFromPublic().test();
    GradFromProtected().test();
    GradFromPrivate().test();
    return 0;
}
