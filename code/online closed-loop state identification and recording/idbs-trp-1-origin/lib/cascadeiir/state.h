#ifndef STATE_H
#define STATE_H

#include <stdexcept>
#include "biquad.h"

class State{
public:
    State();
    virtual void reset() = 0;
    virtual double filter(double in, Biquad* s) = 0;
    virtual ~State() = 0;
};

class DirectFormI : public State{
public:
    DirectFormI();
    ~DirectFormI();

    void reset ();
    double filter(double in, Biquad* s);

protected:
    double m_x2 = 0; // x[n-2]
    double m_y2 = 0; // y[n-2]
    double m_x1 = 0; // x[n-1]
    double m_y1 = 0; // y[n-1]
};

//------------------------------------------------------------------------------


class DirectFormII : public State{
public:
    DirectFormII();
    virtual ~DirectFormII();

    virtual void reset();
    virtual double filter(double in, Biquad* s);

private:
    double m_v1 = 0; // v[-1]
    double m_v2 = 0; // v[-2]
};


//------------------------------------------------------------------------------

class TransposedDirectFormII : public State{
public:
    TransposedDirectFormII();
    virtual ~TransposedDirectFormII();

    virtual void reset();
    virtual double filter(double in, Biquad* s);

private:
    double m_s1 = 0;
    double m_s1_1 = 0;
    double m_s2 = 0;
    double m_s2_1 = 0;
};

//------------------------------------------------------------------------------

enum StateType{
    e_DirectFormI,
    e_DirectFormII,
    e_TransposedDirectFormII,
};

class StateFactory{
public:
    static State* getStateInstance(StateType type);
};


#endif // STATE_H
