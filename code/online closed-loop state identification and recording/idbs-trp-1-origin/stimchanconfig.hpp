#ifndef STIMCHANCONFIG_H
#define STIMCHANCONFIG_H
#include <QTimer>
#include <math.h>
#include "stimdevice.hpp"


const double RAMP_STEP = 0.05;
const int RAMP_STEP_DURATION = 10;
const int STIM_DURATION = 7200;
const int BALANCE_PULSE_TIME = 1;

class StimChanConfig : public QObject
{
    Q_OBJECT
public:
    StimChanConfig(StimDevice *stimulator, StimChan chan, int idx) {
        // Set default value
        StimParm parm;
        parm.amplitude_mA = 0.01;
        parm.pulseWidth_mS = 0.06;
        parm.freq_Hz = 130;

        targetAmpl = 0;
        nRampSteps = 0;
        countRampStep = 0;
        rampStep = 0;

        this->chan = chan;
        this->pStimulator = stimulator;
        this->idx = idx;
        isStimulating = false;

        // Timer for ramp stimulation (step)
        tRamp = new QTimer(this);
        tRamp->setSingleShot(true);
        connect(tRamp, &QTimer::timeout, this, &StimChanConfig::slotTimerRamp);

        // Timer for ramp stimulation (timeout)
        tStim = new QTimer(this);
        tStim->setSingleShot(true);
        connect(tStim, &QTimer::timeout, this, &StimChanConfig::slotRampStimTimeout);

        // Timer for stimulation (timeout)
        tRampStim = new QTimer(this);
        tRampStim->setSingleShot(true);
        connect(tRampStim, &QTimer::timeout, this, &StimChanConfig::slotStimTimeout);
    }

    ~StimChanConfig(){
        disconnect(tRamp, &QTimer::timeout, this, &StimChanConfig::slotTimerRamp);
        disconnect(tStim, &QTimer::timeout, this, &StimChanConfig::slotStimTimeout);
        disconnect(tRampStim, &QTimer::timeout, this, &StimChanConfig::slotRampStimTimeout);
        tRamp->deleteLater();
        tRamp = nullptr;
        tStim->deleteLater();
        tStim = nullptr;
        tRampStim->deleteLater();
        tRampStim = nullptr;
    }

    StimChan getChan(){
        return chan;
    }

    void setParm(StimParm parm){
        this->parm = parm;
    }

    StimParm getStimParm(){
        return parm;
    }

    void stimStarted(double duration_sec){
        this->isStimulating = true;
        if(tStim->isActive()){
            tStim->stop();
        }
        tStim->start(duration_sec*1000);
    }

    void stimStoped(){
        isStimulating = false;
        if(tStim->isActive()){
            tStim->stop();
        }
    }

    void rampStimStarted(double duration_sec){
        this->isStimulating = true;
        if(tRampStim->isActive()){
            tRampStim->stop();
        }
        tRampStim->start(duration_sec*1000);
    }

    void rampStimStoped(){
        isStimulating = false;
        if(tRampStim->isActive()){
            tRampStim->stop();
        }
    }

    bool getStimStatus(){
        return isStimulating;
    }

    void stimStartRamp(StimParm parm){
        if (this->getStimStatus()){
            nRampSteps = ceil(fabs(parm.amplitude_mA-this->getStimParm().amplitude_mA)/RAMP_STEP);
            rampStep = (parm.amplitude_mA-this->getStimParm().amplitude_mA)/nRampSteps;

            this->targetAmpl = parm.amplitude_mA;
            parm.amplitude_mA = this->parm.amplitude_mA+rampStep;

            this->setParm(parm);

            pStimulator->setStimParm(idx, parm);
            pStimulator->startStim(idx, STIM_DURATION);

            tRamp->stop();
            tRamp->start(RAMP_STEP_DURATION);

            this->countRampStep++;
        }else {
            nRampSteps = ceil(parm.amplitude_mA/RAMP_STEP);
            rampStep = parm.amplitude_mA/nRampSteps;

            this->targetAmpl = parm.amplitude_mA;
            parm.amplitude_mA = rampStep;

            this->setParm(parm);

            pStimulator->setStimParm(idx, parm);
            pStimulator->startStim(idx, STIM_DURATION);

            tRamp->stop();
            tRamp->start(RAMP_STEP_DURATION);

            this->countRampStep++;
        }
    }

    void stimStopRamp(){
        if (this->getStimStatus()){
            nRampSteps = ceil(parm.amplitude_mA/RAMP_STEP);
            rampStep = -parm.amplitude_mA/nRampSteps;

            this->targetAmpl = parm.amplitude_mA;
            parm.amplitude_mA = parm.amplitude_mA+rampStep;

            this->setParm(parm);

            pStimulator->setStimParm(idx, parm);
            pStimulator->startStim(idx, STIM_DURATION);

            tRamp->stop();
            tRamp->start(RAMP_STEP_DURATION);

            this->countRampStep++;
        }
    }

public slots:
    void slotStimTimeout(){
        pStimulator->stopStim(idx);
    }

    void slotRampStimTimeout(){
        pStimulator->stopStimRamp(idx);
    }

    void slotTimerRamp(){
        if(countRampStep<(nRampSteps-1)){
            this->parm.amplitude_mA = this->parm.amplitude_mA+this->rampStep;

            pStimulator->setStimParm(idx, parm);
            pStimulator->startStim(idx, STIM_DURATION);
            this->tRamp->start(RAMP_STEP_DURATION);
            this->countRampStep++;
        }else {
            this->tRamp->stop();
            this->countRampStep = 0;

            this->parm.amplitude_mA = this->parm.amplitude_mA+this->rampStep;

            if(this->parm.amplitude_mA<0.001){
                    pStimulator->stopStim(idx);
            }else{
                pStimulator->setStimParm(idx, parm);
                pStimulator->startStim(idx, STIM_DURATION);
            }
        }
    }

private:
    int idx;
    StimChan chan;
    StimParm parm;
    bool isStimulating;
    StimDevice *pStimulator = nullptr;
    bool dedicatedStim;

    QTimer *tRamp = nullptr;
    QTimer *tStim = nullptr;
    QTimer *tRampStim = nullptr;

    double targetAmpl;
    int nRampSteps;
    int countRampStep;
    double rampStep;
};
#endif // STIMCHANCONFIG_H
