// B4 - Alternative concept: new bill types loaded as PLUG-IN RULES from a configuration file
// (rules.txt) instead of being written as new C++ classes.
//   total = base + perDay*days + perHour*hours + perUnit*units
// Compares the time per bill (rule interpretation vs a virtual call), the space for the rules,
// and shows the risk: a bad rule is found only at run time. COMPILE WITH -O2.
// The program reads rules.txt from the current folder; if it is not found it uses the same text
// built into the program.
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <ctime>

const char* EMBEDDED_RULES =
    "# name base perDay perHour perUnit\n"
    "TELECONSULT 200 0 0 15\n"
    "DIALYSIS 1500 0 0 800\n"
    "PHYSIO 300 0 0 450\n"
    "OBSERVATION 500 1200 0 0\n";

// ---------------- the data-driven design ----------------
struct Rule { double base, perDay, perHour, perUnit; };

class RuleBook {
    std::map<std::string, Rule> rules_;
public:
    void load(std::istream& in)                           // throws on a malformed rule: risk found at RUN time
    {
        std::string line; int lineNo = 0;
        while (std::getline(in, line)) {
            ++lineNo;
            if (line.empty() || line[0] == '#') continue;
            std::istringstream ls(line);
            std::string name, f1, f2, f3, f4;
            if (!(ls >> name >> f1 >> f2 >> f3 >> f4))
                throw std::runtime_error("rule file line " + std::to_string(lineNo) + ": expected 5 fields");
            Rule r = { std::stod(f1), std::stod(f2), std::stod(f3), std::stod(f4) };   // may throw invalid_argument
            if (r.base < 0 || r.perDay < 0 || r.perHour < 0 || r.perUnit < 0)
                throw std::runtime_error("rule file line " + std::to_string(lineNo) + ": negative rate");
            rules_[name] = r;
        }
    }
    const Rule* find(const std::string& name) const
    {
        std::map<std::string, Rule>::const_iterator it = rules_.find(name);
        return it == rules_.end() ? 0 : &it->second;
    }
    std::size_t size() const { return rules_.size(); }
};

struct RuleBill { const Rule* rule; double days, hours, units; };
inline double interpret(const RuleBill& b)
{ return b.rule->base + b.rule->perDay * b.days + b.rule->perHour * b.hours + b.rule->perUnit * b.units; }

// ---------------- the compiled design (one C++ class per type) ----------------
class Bill { public: virtual ~Bill() = default; virtual double total() const = 0; };
struct TeleconsultBill : Bill { double units; explicit TeleconsultBill(double u) : units(u) {} double total() const override { return 200 + 15 * units; } };
struct DialysisBill    : Bill { double units; explicit DialysisBill(double u) : units(u) {}    double total() const override { return 1500 + 800 * units; } };
struct PhysioBill      : Bill { double units; explicit PhysioBill(double u) : units(u) {}      double total() const override { return 300 + 450 * units; } };
struct ObservationBill : Bill { double days;  explicit ObservationBill(double d) : days(d) {}  double total() const override { return 500 + 1200 * days; } };

volatile double sink = 0;

int main()
{
    RuleBook book;
    std::ifstream file("rules.txt");
    if (file) { book.load(file); std::cout << "rules loaded from rules.txt: " << book.size() << " rules\n"; }
    else      { std::istringstream in(EMBEDDED_RULES); book.load(in); std::cout << "rules.txt not found: using the built-in copy (" << book.size() << " rules)\n"; }

    std::cout << "sizeof(Rule) = " << sizeof(Rule) << " bytes per rule (data only); sizeof(RuleBill) = " << sizeof(RuleBill)
              << "; sizeof(compiled bill) = " << sizeof(TeleconsultBill) << "\n";
    RuleBill sample = { book.find("TELECONSULT"), 0, 0, 20 };
    std::cout << "TELECONSULT, 20 units -> " << interpret(sample) << " (hand: 200 + 15 x 20 = 500)\n\n";

    // ---- the risk: a bad rule file ----
    const char* badFiles[3] = { "PHYSIO 300 0 0\n", "PHYSIO 300 abc 0 450\n", "PHYSIO 300 0 0 -450\n" };
    for (int i = 0; i < 3; ++i) {
        RuleBook bad;
        std::istringstream in(badFiles[i]);
        try { bad.load(in); std::cout << "bad rule " << i + 1 << " accepted (!)\n"; }
        catch (const std::exception& e) { std::cout << "bad rule " << i + 1 << " rejected at RUN time: " << e.what() << '\n'; }
    }
    std::cout << "(a C++ class with a typo would have been rejected at COMPILE time instead)\n\n";

    // ---- time per bill ----
    const int N = 1000000, REPS = 20;
    const char* names[4] = { "TELECONSULT", "DIALYSIS", "PHYSIO", "OBSERVATION" };
    std::vector<RuleBill> rb;  std::vector<Bill*> vb;  std::vector<std::string> byName;
    for (int i = 0; i < N; ++i) {
        const char* nm = names[i % 4];
        double u = 1 + i % 20;
        RuleBill r = { book.find(nm), (i % 4 == 3) ? u : 0, 0, (i % 4 == 3) ? 0 : u };
        rb.push_back(r);
        byName.push_back(nm);
        switch (i % 4) {
            case 0: vb.push_back(new TeleconsultBill(u)); break;
            case 1: vb.push_back(new DialysisBill(u)); break;
            case 2: vb.push_back(new PhysioBill(u)); break;
            default: vb.push_back(new ObservationBill(u)); break;
        }
    }

    double a = 0, b = 0, c = 0;
    clock_t t0 = clock();
    for (int r = 0; r < REPS; ++r) for (int i = 0; i < N; ++i) a += vb[i]->total();
    clock_t t1 = clock();
    for (int r = 0; r < REPS; ++r) for (int i = 0; i < N; ++i) b += interpret(rb[i]);
    clock_t t2 = clock();
    for (int r = 0; r < REPS / 4; ++r)                                  // slower: looks the rule up by name for every bill
        for (int i = 0; i < N; ++i) { RuleBill x = rb[i]; x.rule = book.find(byName[i]); c += interpret(x); }
    clock_t t3 = clock();
    sink = a + b + c;

    double calls = double(N) * REPS;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << N << " bills x " << REPS << " rounds\n";
    std::cout << "virtual call (compiled classes)         : " << 1000.0 * (t1 - t0) / CLOCKS_PER_SEC << " ms  ("
              << 1e9 * double(t1 - t0) / CLOCKS_PER_SEC / calls << " ns/bill)\n";
    std::cout << "rule interpreted (rule pointer resolved): " << 1000.0 * (t2 - t1) / CLOCKS_PER_SEC << " ms  ("
              << 1e9 * double(t2 - t1) / CLOCKS_PER_SEC / calls << " ns/bill)\n";
    std::cout << "rule looked up by NAME for every bill   : " << 1000.0 * (t3 - t2) / CLOCKS_PER_SEC << " ms  for only "
              << REPS / 4 << " rounds (" << 1e9 * double(t3 - t2) / CLOCKS_PER_SEC / (double(N) * (REPS / 4)) << " ns/bill)\n";
    std::cout << "virtual and interpreted totals equal? " << (a == b ? "yes" : "NO") << '\n';
    for (int i = 0; i < N; ++i) delete vb[i];
    return 0;
}
