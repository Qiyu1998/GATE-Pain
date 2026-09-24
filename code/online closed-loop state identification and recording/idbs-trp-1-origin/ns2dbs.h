#ifndef NS2DBS_H
#define NS2DBS_H
#include "stimstrategy.h"
#include "neuralstate.h"

class NS2DBS : public StimStrategy
{
public:
    NS2DBS(QString configFile);
    ~NS2DBS();

    void stimStrategy(int *state, int arrSize);

    virtual void recieveData(double sample);

private:
    double _amplitude0;
    double _pulseWidth0;
    int _freq0;
    bool _stim0;

    double _amplitude1;
    double _pulseWidth1;
    int _freq1;
    bool _stim1;

    double _amplitude2;
    double _pulseWidth2;
    int _freq2;
    bool _stim2;

    double _amplitude3;
    double _pulseWidth3;
    int _freq3;
    bool _stim3;

    NeuralState *_ns = nullptr;
};

#endif // NS2DBS_H
