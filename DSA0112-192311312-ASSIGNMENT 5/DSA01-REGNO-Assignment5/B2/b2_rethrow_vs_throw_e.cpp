// B2 - "Replace throw; with throw e; and explain the difference (hint: the type of the rethrown object)."
//
//   throw;      rethrows the ORIGINAL exception object: same object, same dynamic type, no copy.
//   throw e;    throws a NEW exception object, copy-initialised from 'e' using the STATIC type of e.
//               If e is declared as 'const std::exception&' the copy is a plain std::exception:
//               the derived part (ClaimError and its message) is SLICED away.
//
// Three cases are run below. PREDICT which outer handler runs for each and what the message is.
#include <iostream>
#include <stdexcept>

class ClaimError : public std::runtime_error {
public:
    static int copies;
    explicit ClaimError(const char* m) : std::runtime_error(m) {}
    ClaimError(const ClaimError& o) : std::runtime_error(o) { ++copies; }
};
int ClaimError::copies = 0;

void origin() { throw ClaimError("exceeds cover"); }

void rethrowSame()                      // handler declared with the BASE type, uses  throw;
{
    try { origin(); }
    catch (const std::exception& e) { std::cout << "  inner log: " << e.what() << '\n'; throw; }
}

void rethrowCopyOfBase()                // handler declared with the BASE type, uses  throw e;
{
    try { origin(); }
    catch (const std::exception& e) { std::cout << "  inner log: " << e.what() << '\n'; throw e; }
}

void rethrowCopyOfDerived()             // handler declared with the DERIVED type, uses  throw e;  (as in the assignment)
{
    try { origin(); }
    catch (const ClaimError& e) { std::cout << "  inner log: " << e.what() << '\n'; throw e; }
}

template <typename F>
void outer(const char* label, F f)
{
    ClaimError::copies = 0;
    std::cout << label << '\n';
    try { f(); }
    catch (const ClaimError& e)  { std::cout << "  outer: caught ClaimError, message '" << e.what() << "'\n"; }
    catch (const std::exception& e) { std::cout << "  outer: caught plain std::exception, message '" << e.what() << "'  (SLICED)\n"; }
    std::cout << "  ClaimError copies made: " << ClaimError::copies << "\n\n";
}

int main()
{
    outer("1) catch (const std::exception&) ... throw;",   rethrowSame);
    outer("2) catch (const std::exception&) ... throw e;", rethrowCopyOfBase);
    outer("3) catch (const ClaimError&)      ... throw e;", rethrowCopyOfDerived);
    return 0;
}
