// A5 - Alternative concept: release memory with 'delete' in every catch block instead of RAII.
// A new error path (UnknownPackageError, mode 3) is added WITHOUT adding a matching catch/delete
// -> the manual version leaks; the unique_ptr version cannot.
#include <iostream>
#include <memory>
#include <stdexcept>

int liveBuffers = 0;

struct Buffer {
    Buffer() { ++liveBuffers; }
    ~Buffer() { --liveBuffers; }
};

class NegativeChargeError : public std::runtime_error { public: using std::runtime_error::runtime_error; };
class ClaimError          : public std::runtime_error { public: using std::runtime_error::runtime_error; };
class UnknownPackageError : public std::runtime_error { public: using std::runtime_error::runtime_error; };

void work(int mode)
{
    if (mode == 1) throw NegativeChargeError("negative");
    if (mode == 2) throw ClaimError("claim");
    if (mode == 3) throw UnknownPackageError("package");     // the "new" error path
}

// ---------------- manual delete in every catch block ----------------
void manual(int mode)
{
    Buffer* b = new Buffer;
    try {
        work(mode);
    } catch (const NegativeChargeError&) { delete b; throw; }   // place to forget #1
      catch (const ClaimError&)          { delete b; throw; }   // place to forget #2
      // UnknownPackageError is not handled here -> b is never deleted (place to forget #3)
    delete b;                                                    // normal path (place to forget #4)
}

// ---------------- RAII ----------------
void raii(int mode)
{
    std::unique_ptr<Buffer> b(new Buffer);
    work(mode);                                                  // nothing else to write
}

template <typename F>
void run(const char* label, F f)
{
    liveBuffers = 0;
    for (int mode = 0; mode <= 3; ++mode) {
        try { f(mode); }
        catch (const std::exception&) {}
    }
    std::cout << label << ": buffers still alive after 4 runs (modes 0,1,2,3) = " << liveBuffers << '\n';
}

int main()
{
    run("manual delete in catch", manual);
    run("RAII (unique_ptr)     ", raii);
    std::cout << "\nlines of cleanup code: manual = 4 delete statements (grows with every new exception type),"
                 " RAII = 0\n";
    return 0;
}
