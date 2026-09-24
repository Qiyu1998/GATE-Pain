#ifndef ALPHALABSNR_H
#define ALPHALABSNR_H

#include "stimdevice.hpp"
#include <QLibrary>
#include "stimchanconfig.hpp"

class AlphaLabSNR : public StimDevice
{
public:
    AlphaLabSNR();
    ~AlphaLabSNR();

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

    void displayError();

private:
    QLibrary *_lib = nullptr;

    vector<StimChanConfig*> _stimChan;
};

#endif // ALPHALABSNR_H
