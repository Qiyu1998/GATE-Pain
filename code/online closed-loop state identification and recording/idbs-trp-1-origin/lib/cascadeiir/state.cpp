#include "state.h"

State::State(){

}

State::~State(){

}


/**
* State for applying a second order section to a sample using Direct Form I
*
* Difference equation:
*
*  y[n] = (b0/a0)*x[n] + (b1/a0)*x[n-1] + (b2/a0)*x[n-2]
*                      - (a1/a0)*y[n-1] - (a2/a0)*y[n-2]
**/
DirectFormI::DirectFormI(){
    m_x1 = 0;
    m_x2 = 0;
    m_y1 = 0;
    m_y2 = 0;
}

DirectFormI::~DirectFormI(){

}

void DirectFormI::reset ()
{
    m_x1 = 0;
    m_x2 = 0;
    m_y1 = 0;
    m_y2 = 0;
}

double DirectFormI::filter(double in, Biquad* s){
    const double out = s->m_b0*in + s->m_b1*m_x1 + s->m_b2*m_x2
    - s->m_a1*m_y1 - s->m_a2*m_y2;
    m_x2 = m_x1;
    m_y2 = m_y1;
    m_x1 = in;
    m_y1 = out;

    return out;
}

//------------------------------------------------------------------------------

/**
* State for applying a second order section to a sample using Direct Form II
*
* Difference equation:
*
*  v[n] =         x[n] - (a1/a0)*v[n-1] - (a2/a0)*v[n-2]
*  y(n) = (b0/a0)*v[n] + (b1/a0)*v[n-1] + (b2/a0)*v[n-2]
*
**/
DirectFormII::DirectFormII(){
    m_v1 = 0;
    m_v2 = 0;
}

DirectFormII::~DirectFormII(){

}

void DirectFormII::reset(){
    m_v1 = 0;
    m_v2 = 0;
}

double DirectFormII::filter(double in, Biquad* s){
    const double w   = in - s->m_a1*m_v1 - s->m_a2*m_v2;
    const double out =      s->m_b0*w    + s->m_b1*m_v1 + s->m_b2*m_v2;

    m_v2 = m_v1;
    m_v1 = w;

    return out;
}

//------------------------------------------------------------------------------

TransposedDirectFormII::TransposedDirectFormII(){
    m_s1 = 0;
    m_s1_1 = 0;
    m_s2 = 0;
    m_s2_1 = 0;
}

TransposedDirectFormII::~TransposedDirectFormII(){

}

void TransposedDirectFormII::reset(){
    m_s1 = 0;
    m_s1_1 = 0;
    m_s2 = 0;
    m_s2_1 = 0;
}

double TransposedDirectFormII::filter(double in,Biquad* s){
    const double out = m_s1_1 + s->m_b0*in;
    m_s1 = m_s2_1 + s->m_b1*in - s->m_a1*out;
    m_s2 = s->m_b2*in - s->m_a2*out;
    m_s1_1 = m_s1;
    m_s2_1 = m_s2;

    return out;
}

//------------------------------------------------------------------------------

State* StateFactory::getStateInstance(StateType type){
    State *s = NULL;
    switch(type){
    case e_DirectFormI:
        s = new DirectFormI;
        break;
    case e_DirectFormII:
        s = new DirectFormII;
        break;
    case e_TransposedDirectFormII:
        s = new TransposedDirectFormII;
        break;
    }
    return s;
}
