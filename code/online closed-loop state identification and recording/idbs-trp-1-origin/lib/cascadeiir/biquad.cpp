#include "biquad.h"

void Biquad::setCoefficients(double a0, double a1, double a2,
                  double b0, double b1, double b2){

    if (is_nan (a0)) throw std::invalid_argument("a0 is NaN");
    if (is_nan (a1)) throw std::invalid_argument("a1 is NaN");
    if (is_nan (a2)) throw std::invalid_argument("a2 is NaN");
    if (is_nan (b0)) throw std::invalid_argument("b0 is NaN");
    if (is_nan (b1)) throw std::invalid_argument("b1 is NaN");
    if (is_nan (b2)) throw std::invalid_argument("b2 is NaN");

    m_a0 = a0;
    m_a1 = a1/a0;
    m_a2 = a2/a0;
    m_b0 = b0/a0;
    m_b1 = b1/a0;
    m_b2 = b2/a0;
}

void Biquad::setIdentity(){
    setCoefficients (1, 0, 0, 1, 0, 0);
}

void Biquad::applyScale(double scale){
    m_b0 *= scale;
    m_b1 *= scale;
    m_b2 *= scale;
}
