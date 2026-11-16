#include <cmath>
#include <iostream>
#include <stdexcept>

class Complex {
private:
    double real;
    double imag;

public:
    Complex() : real(0.0), imag(0.0) {}
    Complex(double r) : real(r), imag(0.0) {}
    Complex(double r, double i) : real(r), imag(i) {}

    Complex operator+(const Complex& z) const {
        return Complex(real + z.real, imag + z.imag);
    }

    Complex operator-(const Complex& z) const {
        return Complex(real - z.real, imag - z.imag);
    }

    Complex operator*(const Complex& z) const {
        return Complex(real * z.real - imag * z.imag,
                       real * z.imag + imag * z.real);
    }

    Complex operator/(const Complex& z) const {
        double denominator = z.real * z.real + z.imag * z.imag;
        if (denominator == 0.0)
            throw std::domain_error("Division by zero complex number");
        return Complex((real * z.real + imag * z.imag) / denominator,
                       (imag * z.real - real * z.imag) / denominator);
    }

    bool operator==(const Complex& z) const {
        return real == z.real && imag == z.imag;
    }

    double magnitude() const {
        return std::sqrt(real * real + imag * imag);
    }

    friend std::ostream& operator<<(std::ostream& out, const Complex& z) {
        out << z.real;
        if (z.imag >= 0) out << " + " << z.imag << 'j';
        else out << " - " << -z.imag << 'j';
        return out;
    }
};

int main() {
    // Example RLC impedances at a selected angular frequency:
    // Resistor: 10 + 0j, inductor: 0 + 5j, capacitor: 0 - 2j.
    Complex resistor(10, 0);
    Complex inductor(0, 5);
    Complex capacitor(0, -2);

    Complex series = resistor + inductor + capacitor;
    std::cout << "Series impedance: " << series
              << "\nMagnitude: " << series.magnitude() << '\n';

    Complex z1(10, 5);
    Complex z2(4, -2);
    try {
        Complex parallel = (z1 * z2) / (z1 + z2);
        std::cout << "Parallel impedance: " << parallel
                  << "\nMagnitude: " << parallel.magnitude() << '\n';

        Complex invalid = z1 / Complex(0, 0);
        std::cout << invalid << '\n';
    } catch (const std::domain_error& ex) {
        std::cout << "Error: " << ex.what() << '\n';
    }
    return 0;
}
