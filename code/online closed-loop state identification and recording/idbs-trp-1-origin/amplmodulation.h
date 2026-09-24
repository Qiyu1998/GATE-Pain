#ifndef AMPLMODULATION_H
#define AMPLMODULATION_H
#include "stimstrategy.h"
#include "cascadeiir/cascadeiir.h"
#include <QFile>
#include <QTextStream>

class AmplModulation : public StimStrategy
{
public:
    AmplModulation(QString configFile);
    ~AmplModulation();
    virtual void recieveData(double sample);

private:
    double calOscillationAmpl();

    int _fs;
    double _freqband[2];

    double _amplitude;
    double _pulseWidth;
    int _freq;

    double _amplLimMax;
    double _amplLimMin;

    double _maxRate;

    int _winSize;
    double *_pBuffer = nullptr;
    int _idxBuffer;

    int _nInit;
    int _idxInit;
    double *_pBufferInit = nullptr;
    bool _isInit;

    double _A;
    double _B;

    QFile *_file = nullptr;
    QTextStream *_stream = nullptr;
};

#endif // AMPLMODULATION_H
