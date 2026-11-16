// A3.1 / A3.2 - Payer interface, derived payers, and an extension without changing anything.
//
// UML:
//         +--------------------------------------------------+
//         |              <<abstract>> Payer                  |
//         | + ~Payer()                           {virtual}   |
//         | + amountCovered(const Bill&) : double {pure virtual}
//         | + name() : string                    {pure virtual}
//         +-------^------------^-------------^---------------+
//                 |            |             |
//        +--------+---+  +-----+--------+  +-+---------------------+
//        |  SelfPay   |  |  Insurance   |  |  GovernmentScheme     |
//        | covers 0   |  | - limit_     |  | - rates_ (code->price)|
//        |            |  | - coPay_ %   |  | pays the FIXED package|
//        +------------+  +--------------+  +-----------------------+
//   EXTENSION (added later, nothing above is edited):   CorporateTieUp : Payer
//
// PSEUDOCODE - patient's share
//   ALGORITHM PatientShare(bill, payer)
//   BEGIN
//       covered <- payer->amountCovered(bill)       // virtual: each payer has its own rule
//       RETURN bill->total() - covered
//   END
#include <iostream>
#include <iomanip>
#include <map>
#include <string>

class Bill {                                           // minimal bill, just enough for the payers
    std::string patient_;
public:
    explicit Bill(const std::string& p) : patient_(p) {}
    virtual ~Bill() = default;
    const std::string& patient() const { return patient_; }
    virtual double total() const = 0;
    virtual std::string type() const = 0;
};
class OutPatientBill : public Bill {
    double c_, t_;
public:
    OutPatientBill(const std::string& p, double c, double t) : Bill(p), c_(c), t_(t) {}
    double total() const override { return c_ + t_; }
    std::string type() const override { return "OP"; }
};
class InPatientBill : public Bill {
    int d_; double r_, s_;
public:
    InPatientBill(const std::string& p, int d, double r, double s) : Bill(p), d_(d), r_(r), s_(s) {}
    double total() const override { return d_ * r_ + s_; }
    std::string type() const override { return "IP"; }
};

// ------------------------------- A3.1 payers -------------------------------
class Payer {
public:
    virtual ~Payer() = default;
    virtual double amountCovered(const Bill& b) const = 0;
    virtual std::string name() const = 0;
};

class SelfPay : public Payer {
public:
    double amountCovered(const Bill&) const override { return 0; }
    std::string name() const override { return "Self pay"; }
};

class Insurance : public Payer {
    double limit_, coPayPercent_;                       // limit_ = cover limit, coPayPercent_ = patient's share
public:
    Insurance(double limit, double coPayPercent) : limit_(limit), coPayPercent_(coPayPercent) {}
    double amountCovered(const Bill& b) const override
    {
        double share = b.total() * (100 - coPayPercent_) / 100;
        return share < limit_ ? share : limit_;         // design: cap at the cover limit
    }
    std::string name() const override { return "Insurance"; }
};

class GovernmentScheme : public Payer {
    std::map<std::string, double> packageRates_;        // bill type -> fixed package rate
public:
    explicit GovernmentScheme(const std::map<std::string, double>& rates) : packageRates_(rates) {}
    double amountCovered(const Bill& b) const override
    {
        std::map<std::string, double>::const_iterator it = packageRates_.find(b.type());
        if (it == packageRates_.end()) return 0;        // design: not in scheme -> nothing covered
        return it->second < b.total() ? it->second : b.total();
    }
    std::string name() const override { return "Govt scheme"; }
};

// ------------------------------- A3.2 extension -------------------------------
// A corporate tie-up pays a fixed percentage of every bill up to a per-bill ceiling.
// Nothing above this line was changed; the function that prints the settlement works as before.
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

void settlement(const Bill& b, const Payer& p)         // closed for modification, open for new payers
{
    double covered = p.amountCovered(b);
    std::cout << std::left << std::setw(4) << b.type() << std::setw(8) << b.patient()
              << std::setw(13) << p.name() << "billed " << std::right << std::setw(8) << b.total()
              << "  covered " << std::setw(8) << covered << "  patient pays " << std::setw(8) << b.total() - covered << '\n';
}

int main()
{
    OutPatientBill op("Ravi", 500, 1200);
    InPatientBill  ip("Meena", 3, 2500, 40000);

    std::map<std::string, double> rates;
    rates["OP"] = 1000; rates["IP"] = 30000;

    SelfPay self;
    Insurance ins(100000, 20);
    GovernmentScheme gov(rates);
    CorporateTieUp corp(90, 25000);                    // the new payer

    const Payer* payers[] = { &self, &ins, &gov, &corp };
    const Bill*  bills[]  = { &op, &ip };
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 4; ++j) settlement(*bills[i], *payers[j]);
    return 0;
}
