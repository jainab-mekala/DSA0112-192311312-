// B1.1 - Validate the implementation: one bill of EACH kind with EACH payer, checked against
// values worked out BY HAND (shown in the comments).
//
// Bills (billed total):
//   OP : 500 + 1200                      = 1,700
//   IP : 3 days x 2,500 + 40,000 + 5,000 = 52,500
//   DC : package price                   = 25,000   (code DC-CATARACT)
//   ER : 3,000 + 2.5 h x 800             = 5,000
// Payers and amount covered:
//   Self pay                          : 0
//   Insurance (limit 100,000, 20% co-pay): 80% of the bill   -> 1,360 | 42,000 | 20,000 | 4,000
//   Government scheme (OP 1,000, IP 30,000, ER 8,000, DC-CATARACT 15,000):
//                                       min(package rate, bill) -> 1,000 | 30,000 | 15,000 | 5,000
#include "billing.h"

int main()
{
    OutPatientBill op("Ravi", 500, 1200);
    InPatientBill  ip("Meena", 3, 2500, 40000, 5000);
    DayCareBill    dc("Kiran", "DC-CATARACT", 25000);
    EmergencyBill  er("Sara", 3000, 2.5, 800);
    const Bill* bills[4] = { &op, &ip, &dc, &er };

    std::map<std::string, double> rates;
    rates["OP"] = 1000; rates["IP"] = 30000; rates["ER"] = 8000; rates["DC-CATARACT"] = 15000;
    SelfPay self;
    Insurance ins(100000, 20);
    GovernmentScheme gov(rates);
    const Payer* payers[3] = { &self, &ins, &gov };

    const double expectedTotal[4]      = { 1700, 52500, 25000, 5000 };
    const double expectedCovered[3][4] = { { 0, 0, 0, 0 },
                                           { 1360, 42000, 20000, 4000 },
                                           { 1000, 30000, 15000, 5000 } };
    int pass = 0, fail = 0;
    std::cout << std::fixed << std::setprecision(2);
    for (int p = 0; p < 3; ++p) {
        for (int b = 0; b < 4; ++b) {
            Settlement s = settle(*bills[b], *payers[p]);
            bool ok = std::fabs(s.billed - expectedTotal[b]) < 1e-9 &&
                      std::fabs(s.covered - expectedCovered[p][b]) < 1e-9 &&
                      std::fabs(s.patientPays - (expectedTotal[b] - expectedCovered[p][b])) < 1e-9;
            std::cout << std::left << std::setw(4) << bills[b]->type() << std::setw(13) << payers[p]->name()
                      << std::right << " billed " << std::setw(9) << s.billed << "  covered " << std::setw(9) << s.covered
                      << "  patient " << std::setw(9) << s.patientPays << "   " << (ok ? "PASS" : "FAIL") << '\n';
            ok ? ++pass : ++fail;
        }
        std::cout << '\n';
    }
    std::cout << pass << " passed, " << fail << " failed\n";
    return fail == 0 ? 0 : 1;
}
