// Q2 - Changed condition: marks stored in a std::vector that may grow.
// A reference/pointer to an element is invalidated when the vector reallocates.
// NOTE: this program never READS through the stale reference (that would be UB);
//       it only compares ADDRESSES to prove that the element has moved.
#include <iostream>
#include <vector>

int& highestRef(std::vector<int>& v)
{
    int idx = 0;
    for (int i = 1; i < (int)v.size(); ++i)
        if (v[i] > v[idx]) idx = i;
    return v[idx];
}

int main()
{
    std::vector<int> marks;
    marks.push_back(56); marks.push_back(78); marks.push_back(91); marks.push_back(64);

    int& top = highestRef(marks);                 // reference obtained BEFORE growth
    const int* oldAddress = &marks[0];
    std::size_t oldCapacity = marks.capacity();
    int topIndex = 2;                             // index of 91 (index stays valid)

    std::cout << "size = " << marks.size() << ", capacity = " << oldCapacity << '\n';
    std::cout << "address of top (reference)    : " << &top << '\n';
    std::cout << "address of marks[2] right now : " << &marks[2] << "\n\n";

    // grow the vector until it must reallocate
    while (marks.capacity() == oldCapacity) marks.push_back(50);

    std::cout << "after growth: size = " << marks.size()
              << ", capacity = " << marks.capacity() << '\n';
    std::cout << "buffer moved?                 : " << (oldAddress != &marks[0] ? "YES" : "NO") << '\n';
    std::cout << "address top refers to (OLD)   : " << &top << '\n';
    std::cout << "address of marks[2] NOW       : " << &marks[topIndex] << '\n';
    std::cout << "=> 'top' still points at the freed old buffer: it is a DANGLING reference.\n";
    std::cout << "=> the index is still valid: marks[" << topIndex << "] = " << marks[topIndex] << "\n\n";

    // safe alternatives
    std::vector<int> safe;
    safe.reserve(100);                            // capacity fixed up front
    safe.push_back(56); safe.push_back(78); safe.push_back(91);
    int& ok = highestRef(safe);
    for (int i = 0; i < 20; ++i) safe.push_back(10);   // no reallocation (size < 100)
    ok += 2;                                           // still valid, because of reserve()
    std::cout << "with reserve(100) the reference stayed valid: highest = " << highestRef(safe) << '\n';
    return 0;
}
