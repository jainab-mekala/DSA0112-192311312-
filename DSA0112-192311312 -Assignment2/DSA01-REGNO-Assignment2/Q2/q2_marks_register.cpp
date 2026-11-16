// Q2 - Examination Marks Register: return by reference
#include <iostream>

// returns a reference to the highest mark ITSELF (an alias for the array element)
int& highestRef(int marks[], int n)
{
    int idx = 0;
    for (int i = 1; i < n; ++i)
        if (marks[i] > marks[idx]) idx = i;
    return marks[idx];
}

// read-only alias: caller can look but not change
const int& highestConstRef(const int marks[], int n)
{
    int idx = 0;
    for (int i = 1; i < n; ++i)
        if (marks[i] > marks[idx]) idx = i;
    return marks[idx];
}

// a copy: caller gets a value, the array is untouched
int highestCopy(const int marks[], int n)
{
    int idx = 0;
    for (int i = 1; i < n; ++i)
        if (marks[i] > marks[idx]) idx = i;
    return marks[idx];
}

void print(const char* label, const int marks[], int n)
{
    std::cout << label;
    for (int i = 0; i < n; ++i) std::cout << marks[i] << ' ';
    std::cout << '\n';
}

int main()
{
    int marks[6] = {56, 78, 91, 64, 85, 72};
    const int N = 6;
    print("original marks               : ", marks, N);

    // 1. return by reference: the call is on the LEFT of the assignment
    highestRef(marks, N) += 2;                 // moderation of +2 applied directly
    print("after highestRef(...) += 2   : ", marks, N);

    highestRef(marks, N) = 90;                 // even a plain assignment works
    print("after highestRef(...) = 90   : ", marks, N);

    // 2. binding a reference variable to the result
    int& top = highestRef(marks, N);
    top = 95;
    print("after top = 95 (alias)       : ", marks, N);

    // 3. return a copy: the array does not change
    int copy = highestCopy(marks, N);
    copy += 2;
    print("after copy += 2 (copy)       : ", marks, N);
    std::cout << "the copy itself is           : " << copy << '\n';

    // 4. const reference: reading is allowed, modifying is a compile error
    const int& ro = highestConstRef(marks, N);
    std::cout << "read through const ref       : " << ro << '\n';
    // highestConstRef(marks, N) += 2;         // <-- remove the comment: ERROR, assignment of read-only reference
    // ro = 5;                                 // <-- ERROR as well

    return 0;
}
