// Q5 - Tickets created inside a function that is called twice.
// Question: what happens to the static count after the local tickets are destroyed?
// PREDICT the output (and the final count) before running.
#include <iostream>
#include <string>

class BusTicket {
    static int issued;
    static const int CAPACITY = 3;
    int seat;
    std::string name;
public:
    explicit BusTicket(const std::string& n) : seat(0), name(n)
    {
        if (issued < CAPACITY) { ++issued; seat = issued; }
    }
    static int count() { return issued; }
    void print() const
    {
        std::cout << "  " << name << ": "
                  << (seat ? "Seat " + std::to_string(seat) : std::string("Waiting")) << '\n';
    }
};
int BusTicket::issued = 0;

void bookTwo(const std::string& tag)
{
    std::cout << tag << " - creating two local tickets\n";
    BusTicket a(tag + "-A");
    BusTicket b(tag + "-B");
    a.print();
    b.print();
    std::cout << "  count inside the function : " << BusTicket::count() << '\n';
}   // a and b are destroyed here, but there is no destructor that gives the seat back

int main()
{
    std::cout << "count before any call     : " << BusTicket::count() << "  (no object exists yet)\n\n";
    bookTwo("Call1");
    std::cout << "count after call 1        : " << BusTicket::count() << "  (both objects are gone)\n\n";
    bookTwo("Call2");
    std::cout << "count after call 2        : " << BusTicket::count() << '\n';
    return 0;
}
