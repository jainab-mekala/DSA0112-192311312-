// Q1 - Hostel Fee Concession Updater: call by value, by reference, by pointer
// PREDICT the stored fee after each call BEFORE running this program.
#include <iostream>

void concessionByValue(long long fee, int percent)
{
    fee -= fee * percent / 100;
    std::cout << "  inside by value     : fee = " << fee
              << "   (address of the parameter copy: " << &fee << ")\n";
}

void concessionByReference(long long& fee, int percent)
{
    fee -= fee * percent / 100;
    std::cout << "  inside by reference : fee = " << fee
              << "   (address of the caller's variable: " << &fee << ")\n";
}

void concessionByPointer(long long* fee, int percent)
{
    if (fee == 0) return;                       // a pointer can be null, a reference cannot
    *fee -= *fee * percent / 100;
    std::cout << "  inside by pointer   : *fee = " << *fee
              << "   (pointer variable's own address: " << &fee
              << ", it points to: " << fee << ")\n";
}

// const reference: read-only access without copying (used when we only need to look)
long long previewConcession(const long long& fee, int percent)
{
    return fee - fee * percent / 100;
}

int main()
{
    long long f1 = 45000, f2 = 45000, f3 = 45000;

    std::cout << "address of f1 (by value test)    : " << &f1 << '\n';
    std::cout << "address of f2 (by reference test): " << &f2 << '\n';
    std::cout << "address of f3 (by pointer test)  : " << &f3 << "\n\n";

    std::cout << "Merit concession (10%) applied to each:\n";
    concessionByValue(f1, 10);
    std::cout << "  stored fee after by value     : " << f1 << "\n\n";

    concessionByReference(f2, 10);
    std::cout << "  stored fee after by reference : " << f2 << "\n\n";

    concessionByPointer(&f3, 10);
    std::cout << "  stored fee after by pointer   : " << f3 << "\n\n";

    long long f4 = 45000;
    std::cout << "preview of 25% single-parent concession (const ref): "
              << previewConcession(f4, 25) << '\n';
    std::cout << "f4 is still                                         : " << f4 << '\n';
    return 0;
}
