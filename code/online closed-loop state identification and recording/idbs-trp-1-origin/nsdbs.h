#ifndef NSDBS_H
#define NSDBS_H
#include "stimstrategy.h"
#include "neuralstate.h"

class NSDBS : public StimStrategy
{
public:
    NSDBS(QString configFile);
    ~NSDBS();

    void stimStrategy(int *state, int arrSize);

    virtual void recieveData(double sample);

private:
    double _amplitude;
    double _pulseWidth;
    int _freq;

    NeuralState *_ns = nullptr;
};

#endif // NSDBS_H
