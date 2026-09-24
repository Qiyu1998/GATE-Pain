#ifndef AOTEST_H
#define AOTEST_H

#include <QTimer>
#include <time.h>
#include "stimdevice.hpp"

class AOTest : public StimDevice
{
public:
    AOTest();
    ~AOTest();

    static void getSamplingRateList(int* pSRList);
    static int getSamplingRateNumber();

    virtual int connect();
    virtual int isConnected();
    virtual int disconnect();

    virtual int addBufferChannel(long channelId, int bufferSizemSec);
    virtual int getChannelData(int channelId, short *pData, int arrSizeWords,int *dataCaptured);
    virtual int clearBuffers();

    virtual int addRecordingChannel(int channelID);
    virtual int startRecording();
    virtual int stopRecording();

    virtual int setStimChan(vector<StimChan> chan, bool ExclusiveMode=false);

    virtual int setStimParm(int idx, StimParm stimParm);
    virtual int startStim(int idx, double duration_sec);
    virtual int stopStim(int idx);
    virtual int stopStim();

    virtual void startStimRamp(int idx, StimParm stimParm, double duration_sec);
    virtual void stopStimRamp(int idx);
    virtual void stopStimRamp();

public slots:
    void genData();

private:
    int _samplingRate;
    int _isConnected;
    short *_pData = nullptr;
    QTimer *_tGenData = nullptr;
    int _bufferSizePoints;
};

#endif // AOTEST_H
