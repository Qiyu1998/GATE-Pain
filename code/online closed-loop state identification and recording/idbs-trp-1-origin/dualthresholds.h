#ifndef DUALTHRESHOLDS_H
#define DUALTHRESHOLDS_H
#include "stimstrategy.h"
#include "cascadeiir/cascadeiir.h"
#include <QFile>
#include <QTextStream>

class DualThresholds : public StimStrategy
{
public:
    DualThresholds(QString configFile);
    ~DualThresholds();

    virtual void recieveData(double sample);

private:
    void getThreshold();
    double calOscillationAmpl();

    int _fs;
    double _freqband[2];

    double _amplitude;
    double _pulseWidth;
    int _freq;

    double _amplLimMax;
    double _amplLimMin;

    double _adjustStep;
    int _winSize;
    double *_pBuffer = nullptr;
    int _idxBuffer;

    int _nInit;
    int _idxInit;
    double *_pBufferInit = nullptr;
    bool _isInit;

    double _thrUpper;
    double _thrLower;

    QFile *_file = nullptr;
    QTextStream *_stream = nullptr;

};

#endif // DUALTHRESHOLDS_H
