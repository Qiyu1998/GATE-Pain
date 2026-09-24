#ifndef THRADBS_H
#define THRADBS_H
#include "stimstrategy.h"
#include "cascadeiir/cascadeiir.h"
#include <QFile>
#include <QTextStream>

class THRaDBS : public StimStrategy
{
public:
    THRaDBS(QString configFile);
    ~THRaDBS();

    virtual void recieveData(double sample);
    void shiftBuffer(double sample);

private:
    double _amplitude;
    double _pulseWidth;
    int _freq;
    double _threshold;

    CascadeIIR *_filter = nullptr;
    double **_sosMat = nullptr;
    int _nSOS = 0;

    double *_pBuffer = nullptr;
    int _winSize;

    QFile *_file = nullptr;
    QTextStream *_stream = nullptr;

    int _nInit;
    int _idxInit;
    double *_pBufferInit = nullptr;
    bool _isInit;
};

#endif // THRADBS_H
