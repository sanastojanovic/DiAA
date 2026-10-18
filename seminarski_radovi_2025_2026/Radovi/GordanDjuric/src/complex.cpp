#include <cmath>

#include "complex.hpp"


float Complex::module() const
{
    return std::sqrt(this->m_real*this->m_real + this->m_imag*this->m_imag);
}

float Complex::real() const
{
    return m_real;
}

float Complex::imag() const
{
    return m_imag;
}

Complex Complex::operator+(const Complex& other ) const {
    return Complex(other.m_real+this->m_real,other.m_imag+this->m_imag);
}

Complex Complex::operator-(const Complex& other ) const {
    return Complex(-other.m_real+this->m_real,-other.m_imag+this->m_imag);
}

Complex Complex::operator*(const Complex& other ) const {
    return Complex(this->m_real*other.m_real-this->m_imag*other.m_imag,this->m_real*other.m_imag+other.m_real*this->m_imag);
}

Complex Complex::operator*(const float skalar) const {
    return Complex(this->m_real*skalar,this->m_imag*skalar);
}

Complex Complex::operator/(const Complex& other ) const {
    return (*this)*(~other)*(Complex(1/pow(other.module(),2),0));
}

Complex Complex::operator~() const {
    return Complex(this->m_real,-this->m_imag);
}


std::ostream &operator<<(std::ostream &out, const Complex &obj)
{
    out << obj.m_real << " + " << obj.m_imag << "i";
    return out;
}

std::istream &operator>>(std::istream &in, Complex &obj)
{
    in >> obj.m_real >> obj.m_imag;
    return in;
}

Complex round_complex(Complex c)
{
    float _base = 10000.0f;
    c.m_imag = roundf64(c.m_imag*_base)/_base;
    c.m_real = roundf64(c.m_real*_base)/_base;
    return c;
}
