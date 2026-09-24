#ifndef STIMSTRATEGY_H
#define STIMSTRATEGY_H

#include <QObject>

class StimStrategy:public QObject
{
    Q_OBJECT

public:
    virtual void recieveData(double sample) = 0;

signals:
    void signalStimulation(double amplitude, double pulseWidth, int freq, bool stimOnOff);
};

#endif // STIMSTRATEGY_H
