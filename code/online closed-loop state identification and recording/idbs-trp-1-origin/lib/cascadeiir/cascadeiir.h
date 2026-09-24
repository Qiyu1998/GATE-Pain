#ifndef CASCADEIIR_H
#define CASCADEIIR_H

#include "state.h"
#include <stdexcept>

class CascadeIIR
{
public:
    CascadeIIR(int nSOS, double **sosMat, StateType type = StateType::e_DirectFormII);
    CascadeIIR(int nSOS, const double sosMat[][6], StateType type = StateType::e_DirectFormII);
    ~CascadeIIR();

    void reset();
    double filter(double in);

private:
    int m_num;
    Biquad **m_stages = NULL;
    State **m_states = NULL;
};

#endif // CASCADEIIR_H
