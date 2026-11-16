// Q2 - Returning a reference to a LOCAL variable (dangling reference)
//
// 1. Compile this file with  g++ -std=c++11 -Wall -Wextra q2_dangling_local.cpp
//    and COPY / SCREENSHOT the warning ("reference to local variable ... returned")
//    as your compiler-warning evidence.
// 2. By default the dangerous call is NOT executed. Change RUN_UNDEFINED_BEHAVIOUR
//    to true to try it. The result is undefined behaviour: it may print garbage,
//    print the old value, or crash (newer GCC at -O2 often crashes).
#include <iostream>

const bool RUN_UNDEFINED_BEHAVIOUR = false;

int& badMarksRef()
{
    int local = 99;          // lives on the stack frame of badMarksRef()
    return local;            // WARNING: the frame is destroyed on return
}

int goodMarks()              // the safe way: return a copy
{
    int local = 99;
    return local;
}

int& goodRefToStatic()       // also safe: static storage outlives the call
{
    static int kept = 99;
    return kept;
}

int main()
{
    std::cout << "safe copy            : " << goodMarks() << '\n';
    std::cout << "safe static reference: " << goodRefToStatic() << '\n';

    if (RUN_UNDEFINED_BEHAVIOUR) {
        int& r = badMarksRef();                     // r now dangles
        std::cout << "dangling reference   : " << r << "   <-- undefined behaviour\n";
        r = 5;                                      // writing through it is worse
        std::cout << "after writing r = 5 : " << r << '\n';
    } else {
        std::cout << "(dangling call skipped: set RUN_UNDEFINED_BEHAVIOUR = true to try it)\n";
    }
    return 0;
}
