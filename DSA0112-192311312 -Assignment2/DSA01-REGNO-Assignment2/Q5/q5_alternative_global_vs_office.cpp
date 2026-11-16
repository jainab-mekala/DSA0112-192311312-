// Q5 - Alternative concept: global counter, or a TicketOffice object passed to every
// ticket, instead of a static data member. All are O(1) time, one counter of space.
// Key difference: can TWO buses run at the same time?
#include <iostream>
#include <string>

const int CAPACITY = 3;

// ---------- design 1: static data member (class-level counter) ----------
class TicketStatic {
    static int issued;
    int seat;
public:
    TicketStatic() : seat(0) { if (issued < CAPACITY) seat = ++issued; }
    int  getSeat() const { return seat; }
};
int TicketStatic::issued = 0;

// ---------- design 2: global variable (anyone can change it) ------------
int globalIssued = 0;
class TicketGlobal {
    int seat;
public:
    TicketGlobal() : seat(0) { if (globalIssued < CAPACITY) seat = ++globalIssued; }
    int getSeat() const { return seat; }
};

// ---------- design 3: separate TicketOffice object ----------------------
class TicketOffice {
    int issued, capacity;
public:
    explicit TicketOffice(int cap) : issued(0), capacity(cap) {}
    int nextSeat() { return issued < capacity ? ++issued : 0; }
    int count() const { return issued; }
};
class TicketOff {
    int seat;
public:
    explicit TicketOff(TicketOffice& office) : seat(office.nextSeat()) {}
    int getSeat() const { return seat; }
};

std::string seatText(int s) { return s ? "Seat " + std::to_string(s) : std::string("Waiting"); }

int main()
{
    std::cout << "Two buses, each with " << CAPACITY << " seats, 2 passengers booked on each:\n\n";

    std::cout << "Static data member (one counter shared by ALL tickets):\n";
    TicketStatic a1, a2, b1, b2;                 // a1,a2 -> bus A ; b1,b2 -> bus B
    std::cout << "  bus A: " << seatText(a1.getSeat()) << ", " << seatText(a2.getSeat()) << '\n';
    std::cout << "  bus B: " << seatText(b1.getSeat()) << ", " << seatText(b2.getSeat())
              << "   <-- bus B is wrongly affected by bus A\n\n";

    std::cout << "Global variable (same problem, and any code can modify it):\n";
    TicketGlobal ga1, ga2, gb1, gb2;
    std::cout << "  bus A: " << seatText(ga1.getSeat()) << ", " << seatText(ga2.getSeat()) << '\n';
    std::cout << "  bus B: " << seatText(gb1.getSeat()) << ", " << seatText(gb2.getSeat()) << "\n\n";

    std::cout << "TicketOffice object per bus (one counter per bus):\n";
    TicketOffice busA(CAPACITY), busB(CAPACITY);
    TicketOff oa1(busA), oa2(busA), ob1(busB), ob2(busB);
    std::cout << "  bus A: " << seatText(oa1.getSeat()) << ", " << seatText(oa2.getSeat()) << '\n';
    std::cout << "  bus B: " << seatText(ob1.getSeat()) << ", " << seatText(ob2.getSeat()) << '\n';
    std::cout << "  issued A = " << busA.count() << ", issued B = " << busB.count() << '\n';
    return 0;
}
