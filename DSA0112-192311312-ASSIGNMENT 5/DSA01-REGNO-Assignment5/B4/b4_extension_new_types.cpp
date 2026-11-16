// B4.1 - Practical extension: ONE new bill type (TeleconsultBill) and ONE new payer
// (CorporateTieUp) added WITHOUT changing any existing class. billing.h is included exactly as it
// is (an identical copy of the B1 file). The first block re-runs the earlier B1.1 test table to
// show that all earlier tests still pass.
#include "billing.h"

// ======================= NEW CODE (nothing above this line is edited) =======================
class TeleconsultBill : public Bill {
    double minutes_, ratePerMinute_, platformFee_;
public:
    TeleconsultBill(const std::string& p, double minutes, double ratePerMinute, double platformFee)
        : Bill(p), minutes_(nonNegative(minutes, "minutes")), ratePerMinute_(nonNegative(ratePerMinute, "rate")),
          platformFee_(nonNegative(platformFee, "platform fee")) {}
    double total() const override { return minutes_ * ratePerMinute_ + platformFee_; }
    std::string type() const override { return "TC"; }
};

class CorporateTieUp : public Payer {
    double percent_, ceiling_;
public:
    CorporateTieUp(double percent, double ceiling) : percent_(percent), ceiling_(ceiling) {}
    double amountCovered(const Bill& b) const override
    {
        double c = b.total() * percent_ / 100;
        return c < ceiling_ ? c : ceiling_;
    }
    std::string name() const override { return "Corporate"; }
};
// ===========================================================================================

static int passed = 0, failed = 0;

static void check(const char* label, double actual, double expected)
{
    bool ok = std::fabs(actual - expected) < 1e-9;
    ok ? ++passed : ++failed;
    std::cout << "  " << std::left << std::setw(46) << label << std::right << std::setw(10) << actual
              << "  expected " << std::setw(10) << expected << "  " << (ok ? "PASS" : "FAIL") << '\n';
}

int main()
{
    std::cout << std::fixed << std::setprecision(2);

    // ---------- earlier tests (B1.1): 4 bills x 3 payers ----------
    std::cout << "EARLIER TESTS (B1.1)\n";
    OutPatientBill op("Ravi", 500, 1200);
    InPatientBill  ip("Meena", 3, 2500, 40000, 5000);
    DayCareBill    dc("Kiran", "DC-CATARACT", 25000);
    EmergencyBill  er("Sara", 3000, 2.5, 800);
    const Bill* bills[4] = { &op, &ip, &dc, &er };
    std::map<std::string, double> rates;
    rates["OP"] = 1000; rates["IP"] = 30000; rates["ER"] = 8000; rates["DC-CATARACT"] = 15000;
    SelfPay self; Insurance ins(100000, 20); GovernmentScheme gov(rates);
    const Payer* payers[3] = { &self, &ins, &gov };
    const double expected[3][4] = { { 0, 0, 0, 0 }, { 1360, 42000, 20000, 4000 }, { 1000, 30000, 15000, 5000 } };
    for (int p = 0; p < 3; ++p)
        for (int b = 0; b < 4; ++b) {
            std::string label = bills[b]->type() + " with " + payers[p]->name();
            check(label.c_str(), settle(*bills[b], *payers[p]).covered, expected[p][b]);
        }
    int earlierPassed = passed, earlierFailed = failed;

    // ---------- new tests ----------
    std::cout << "\nNEW TYPE AND NEW PAYER\n";
    TeleconsultBill tc("Anu", 20, 15, 50);                       // 20 x 15 + 50 = 350
    check("TC total", tc.total(), 350);
    check("TC with Self pay", settle(tc, self).covered, 0);
    check("TC with Insurance (80%)", settle(tc, ins).covered, 280);

    std::map<std::string, double> rates2 = rates; rates2["TC"] = 300;
    GovernmentScheme gov2(rates2);
    check("TC with Govt scheme (rate 300)", settle(tc, gov2).covered, 300);

    CorporateTieUp corp(90, 25000);                              // 90% up to 25,000 per bill
    const double corpExpected[4] = { 1530, 25000, 22500, 4500 };  // 90% of 1700 | 47,250 capped | 90% of 25,000 | 90% of 5,000
    for (int b = 0; b < 4; ++b) {
        std::string label = bills[b]->type() + " with Corporate";
        check(label.c_str(), settle(*bills[b], corp).covered, corpExpected[b]);
    }
    check("TC with Corporate (90%)", settle(tc, corp).covered, 315);

    // new type must still obey the error design
    try { settle(tc, gov); std::cout << "  TC with old Govt scheme: no error  FAIL\n"; ++failed; }
    catch (const UnknownPackageError& e) { std::cout << "  TC with old Govt scheme -> UnknownPackageError (" << e.what() << ")  PASS\n"; ++passed; }
    try { TeleconsultBill bad("Anu", -5, 15, 50); std::cout << "  negative minutes accepted  FAIL\n"; ++failed; }
    catch (const NegativeChargeError& e) { std::cout << "  negative minutes -> NegativeChargeError (" << e.what() << ")  PASS\n"; ++passed; }

    std::cout << "\nearlier tests : " << earlierPassed << " passed, " << earlierFailed << " failed\n";
    std::cout << "all tests     : " << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
