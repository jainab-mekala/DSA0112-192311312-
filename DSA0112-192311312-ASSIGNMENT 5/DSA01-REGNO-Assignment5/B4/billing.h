// ============================================================================
// billing.h - Hospital billing: bills, payers and exceptions (header only)
// Used by every B-task program in this folder (B4 holds an identical copy).
//
// CHANGES FROM THE PART A DESIGN (B1.2 - explain these in your report)
//   1. Every bill constructor validates its charges and throws NegativeChargeError;
//      an empty patient name throws MissingRecordError (Part A only listed the errors).
//   2. Insurance::amountCovered() REJECTS a claim above the cover limit with ClaimError instead of
//      silently capping it (the case study says invalid claims must be rejected with a reason).
//   3. GovernmentScheme throws UnknownPackageError for a package code that is not in its table
//      (A3 returned 0). Bills therefore carry a packageCode().
//   4. InPatientBill got a separate 'medicines' charge and DayCare/Emergency bills were added.
// ============================================================================
#ifndef BILLING_H
#define BILLING_H

#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

// ----------------------------- exceptions -----------------------------
class BillingError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
class NegativeChargeError : public BillingError { public: using BillingError::BillingError; };
class ClaimError          : public BillingError { public: using BillingError::BillingError; };
class UnknownPackageError : public BillingError { public: using BillingError::BillingError; };
class MissingRecordError  : public BillingError { public: using BillingError::BillingError; };

inline double nonNegative(double v, const char* what)
{
    if (v < 0) throw NegativeChargeError(std::string("negative ") + what);
    return v;
}

// ----------------------------- bills -----------------------------
class Bill {
    std::string patient_;
public:
    explicit Bill(const std::string& patient) : patient_(patient)
    {
        if (patient_.empty()) throw MissingRecordError("missing patient record");
    }
    virtual ~Bill() = default;
    const std::string& patient() const { return patient_; }
    virtual double total() const = 0;
    virtual std::string type() const = 0;
    virtual std::string packageCode() const { return type(); }     // key used by government schemes
    void print() const
    {
        std::cout << std::left << std::setw(4) << type() << std::setw(10) << patient_
                  << std::right << std::setw(10) << total() << '\n';
    }
};

class OutPatientBill : public Bill {
    double consultation_, tests_;
public:
    OutPatientBill(const std::string& p, double consultation, double tests)
        : Bill(p), consultation_(nonNegative(consultation, "consultation")), tests_(nonNegative(tests, "tests")) {}
    double total() const override { return consultation_ + tests_; }
    std::string type() const override { return "OP"; }
};

class InPatientBill : public Bill {
    int days_; double perDay_, procedures_, medicines_;
public:
    InPatientBill(const std::string& p, int days, double perDay, double procedures, double medicines)
        : Bill(p), days_(days), perDay_(nonNegative(perDay, "room rate")),
          procedures_(nonNegative(procedures, "procedures")), medicines_(nonNegative(medicines, "medicines"))
    {
        if (days < 0) throw NegativeChargeError("negative days");
    }
    double total() const override { return days_ * perDay_ + procedures_ + medicines_; }
    std::string type() const override { return "IP"; }
};

class DayCareBill : public Bill {
    std::string code_; double price_;
public:
    DayCareBill(const std::string& p, const std::string& code, double price)
        : Bill(p), code_(code), price_(nonNegative(price, "package price")) {}
    double total() const override { return price_; }
    std::string type() const override { return "DC"; }
    std::string packageCode() const override { return code_; }
};

class EmergencyBill : public Bill {
    double base_, hours_, hourlyRate_;
public:
    EmergencyBill(const std::string& p, double base, double hours, double hourlyRate)
        : Bill(p), base_(nonNegative(base, "base charge")), hours_(nonNegative(hours, "hours")),
          hourlyRate_(nonNegative(hourlyRate, "hourly rate")) {}
    double total() const override { return base_ + hours_ * hourlyRate_; }
    std::string type() const override { return "ER"; }
};

// ----------------------------- payers -----------------------------
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
    double limit_, coPayPercent_;
public:
    Insurance(double limit, double coPayPercent)
        : limit_(nonNegative(limit, "cover limit")), coPayPercent_(nonNegative(coPayPercent, "co-payment")) {}
    double amountCovered(const Bill& b) const override
    {
        if (b.total() > limit_) throw ClaimError("claim " + std::to_string(b.total()) + " exceeds cover " + std::to_string(limit_));
        return b.total() * (100 - coPayPercent_) / 100;
    }
    std::string name() const override { return "Insurance"; }
};

class GovernmentScheme : public Payer {
    std::map<std::string, double> rates_;
public:
    explicit GovernmentScheme(const std::map<std::string, double>& rates) : rates_(rates) {}
    double amountCovered(const Bill& b) const override
    {
        std::map<std::string, double>::const_iterator it = rates_.find(b.packageCode());
        if (it == rates_.end()) throw UnknownPackageError("unknown package code '" + b.packageCode() + "'");
        return it->second < b.total() ? it->second : b.total();
    }
    std::string name() const override { return "Govt scheme"; }
};

struct Settlement { double billed, covered, patientPays; };

inline Settlement settle(const Bill& b, const Payer& p)
{
    double t = b.total();
    double c = p.amountCovered(b);
    Settlement s = { t, c, t - c };
    return s;
}

#endif
