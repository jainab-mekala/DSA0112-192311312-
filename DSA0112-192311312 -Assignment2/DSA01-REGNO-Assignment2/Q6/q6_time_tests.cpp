// Q6 - Correctness check: same time twice, times on either side of noon.
// Work out each expected value BY HAND first, then compare with the program.
#include <iostream>

class Time {
    int h, m;
public:
    Time(int hh, int mm) : h(hh), m(mm) {}
    friend int minutesBetween(const Time& a, const Time& b);
    friend bool earlier(const Time& a, const Time& b);
};

int minutesBetween(const Time& a, const Time& b)
{
    int ma = a.h * 60 + a.m, mb = b.h * 60 + b.m;
    return ma > mb ? ma - mb : mb - ma;
}
bool earlier(const Time& a, const Time& b) { return a.h * 60 + a.m < b.h * 60 + b.m; }

void test(const char* label, Time a, Time b)
{
    std::cout << label << "\n   minutesBetween = " << minutesBetween(a, b)
              << ",  earlier(a,b) = " << std::boolalpha << earlier(a, b)
              << ",  earlier(b,a) = " << earlier(b, a) << "\n";
}

int main()
{
    test("9:45  vs 13:10 (given example)",        Time(9, 45),  Time(13, 10));
    test("same time twice: 10:30 vs 10:30",       Time(10, 30), Time(10, 30));
    test("either side of noon: 11:50 vs 12:20",   Time(11, 50), Time(12, 20));
    test("across noon, minutes smaller: 11:59 vs 12:01", Time(11, 59), Time(12, 1));
    test("midnight and last minute: 0:00 vs 23:59", Time(0, 0), Time(23, 59));
    return 0;
}
