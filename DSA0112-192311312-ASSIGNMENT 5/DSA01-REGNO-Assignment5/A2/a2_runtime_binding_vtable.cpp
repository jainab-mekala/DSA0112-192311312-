// A2.1 / A2.2 - Early vs late binding and the virtual table.
//
// VTABLE DIAGRAMS (typical layout, one vptr at the start of each object):
//
//   Bill (abstract)               OutPatientBill                 InPatientBill
//   vtable:                       vtable:                        vtable:
//   [0] ~Bill()                   [0] ~OutPatientBill()          [0] ~InPatientBill()
//   [1] total()  = pure (0)       [1] OutPatientBill::total()    [1] InPatientBill::total()
//   [2] type()   = pure (0)       [2] OutPatientBill::type()     [2] InPatientBill::type()
//
//   OutPatientBill object          b->total() :
//   +--------+                       1. read the object's vptr
//   | vptr --+--> OutPatientBill     2. index the vtable slot for total()  (slot 1)
//   | data   |    vtable             3. call whatever function address is stored there
//   +--------+
//
// The program shows: a non-virtual call is bound at COMPILE time (by the pointer's type),
// a virtual call at RUN time (by the object's type); objects of the same class share one
// vptr value; and a hand-made vtable made of function pointers.
#include <iostream>
#include <cstring>

struct Bill {
    virtual ~Bill() = default;
    virtual double total() const = 0;
    virtual const char* type() const = 0;
    const char* kind() const { return "Bill::kind (non-virtual)"; }
};
struct OutPatientBill : Bill {
    double total() const override { return 1700; }
    const char* type() const override { return "OP"; }
    const char* kind() const { return "OutPatientBill::kind (non-virtual, hides Bill::kind)"; }
};
struct InPatientBill : Bill {
    double total() const override { return 47500; }
    const char* type() const override { return "IP"; }
    const char* kind() const { return "InPatientBill::kind (non-virtual, hides Bill::kind)"; }
};

void* vptrOf(const Bill& b)                       // copy the first pointer-sized bytes of the object
{
    void* p;
    std::memcpy(&p, &b, sizeof(p));
    return p;
}

// ---- a hand-made vtable to show the mechanism ----
struct MiniBill;
struct MiniVTable {
    double      (*total)(const MiniBill*);
    const char* (*type)(const MiniBill*);
};
struct MiniBill { const MiniVTable* vt; };         // the "vptr"
double      opTotal(const MiniBill*)  { return 1700; }
const char* opType(const MiniBill*)   { return "OP"; }
double      ipTotal(const MiniBill*)  { return 47500; }
const char* ipType(const MiniBill*)   { return "IP"; }
const MiniVTable opTable = { opTotal, opType };
const MiniVTable ipTable = { ipTotal, ipType };

int main()
{
    OutPatientBill op1, op2;
    InPatientBill  ip;
    Bill* b = &op1;                                // pointer type: Bill*, object type: OutPatientBill

    std::cout << "--- early vs late binding through a Bill* ---\n";
    std::cout << "b->kind()  (non-virtual) : " << b->kind() << '\n';      // decided by the TYPE OF THE POINTER
    std::cout << "b->type()  (virtual)     : " << b->type() << '\n';      // decided by the TYPE OF THE OBJECT
    b = &ip;
    std::cout << "b now points to InPatientBill:\n";
    std::cout << "b->kind()  (non-virtual) : " << b->kind() << '\n';
    std::cout << "b->type()  (virtual)     : " << b->type() << "   total " << b->total() << "\n\n";

    std::cout << "--- the hidden vptr ---\n";
    std::cout << "sizeof(OutPatientBill) = " << sizeof(OutPatientBill) << " (just the vptr: the class has no data)\n";
    std::cout << "vptr of op1 == vptr of op2 ? " << (vptrOf(op1) == vptrOf(op2) ? "yes (same class, same vtable)" : "no") << '\n';
    std::cout << "vptr of op1 == vptr of ip  ? " << (vptrOf(op1) == vptrOf(ip)  ? "yes" : "no (different class, different vtable)") << "\n\n";

    std::cout << "--- hand-made vtable ---\n";
    MiniBill m1 = { &opTable }, m2 = { &ipTable };
    const MiniBill* all[2] = { &m1, &m2 };
    for (int i = 0; i < 2; ++i)
        std::cout << all[i]->vt->type(all[i]) << " total " << all[i]->vt->total(all[i]) << '\n';
    return 0;
}
