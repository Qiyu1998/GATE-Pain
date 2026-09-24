#ifndef BIQUAD_H
#define BIQUAD_H

#include "mathlib.h"

/*
* Holds coefficients for a second order Infinite Impulse Response
* digital filter. This is the building block for all IIR filters.
*
*/
class Biquad
{
public:
    /**
     * Sets all coefficients
     * \param a0 1st IIR coefficient
     * \param a1 2nd IIR coefficient
     * \param a2 3rd IIR coefficient
     *
     * \param b0 1st FIR coefficient
     * \param b1 2nd FIR coefficient
     * \param b2 3rd FIR coefficient
     **/
    void setCoefficients (double a0, double a1, double a2,
                  double b0, double b1, double b2);


    /**
     * Sets the coefficiens as pass through. (b0=1,a0=1, rest zero)
     **/
    void setIdentity ();

    /**
     * Performs scaling operation on the FIR coefficients
     * \param scale Mulitplies the coefficients b0,b1,b2 with the scaling factor scale.
     **/
    void applyScale (double scale);


    double m_a0 = 1;
    double m_a1 = 0;
    double m_a2 = 0;
    double m_b1 = 0;
    double m_b2 = 0;
    double m_b0 = 1;
};

#endif // BIQUAD_H
