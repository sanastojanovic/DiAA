#ifndef __COMPLEX_HPP
#define __COMPLEX_HPP

#include <cmath>
#include <iostream>

class Complex{
    public:
        Complex(): m_real(0.0f), m_imag(0.0f) {}
        Complex(float x_mod, float y_angle, bool polar = false): m_real(x_mod), m_imag(y_angle){
            if (polar)
            {
                m_real = x_mod*std::cos(y_angle);
                m_imag = x_mod*std::sin(y_angle);
            }
        }

        ~Complex() {}

        //functions for complex numbers
        float module() const;
        float angle() const;
        float real() const;
        float imag() const;

        // operators
        Complex operator+(const Complex& other) const;
        Complex operator-(const Complex& other) const;
        Complex operator*(const Complex& other) const;
        Complex operator*(const float skalar) const;
        Complex operator/(const Complex& other) const;
        Complex operator~() const;
        bool operator==(const Complex& other) const;

        friend std::ostream& operator<<(std::ostream& out, const Complex &obj);        
        friend std::istream& operator>>(std::istream& in, Complex &obj);

        friend Complex round_complex(Complex c);

    private:
        float m_real, m_imag;
};

#endif
