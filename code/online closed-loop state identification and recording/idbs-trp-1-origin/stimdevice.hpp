#ifndef STIMDEVICE_H
#define STIMDEVICE_H

#include <QObject>
#include <vector>

using namespace std;

struct StimChan{
    int stimChanID;
    int returnChanID;
};

struct StimParm{
    double amplitude_mA;
    double pulseWidth_mS;
    int freq_Hz;
};

extern "C"
{
    typedef struct MAC_ADDR {
        int  addr[6];
    } MAC_ADDR;
    typedef enum { eSin=1 } ESignalTypes;
    //used for the call back function
    typedef void (*AOParseFunction)(short *pData, int *dataSize);
    //this function will be used as call back function to get back Error messages
    //typedef void (*AODisplayError)(char* Pmessage, int dataSize);
}

class StimDevice:public QObject
{
    Q_OBJECT
public:
    StimDevice(){};

    int setMacAddress(MAC_ADDR macAdd){
        _macAdd.addr[0] = macAdd.addr[0];
        _macAdd.addr[1] = macAdd.addr[1];
        _macAdd.addr[2] = macAdd.addr[2];
        _macAdd.addr[3] = macAdd.addr[3];
        _macAdd.addr[4] = macAdd.addr[4];
        _macAdd.addr[5] = macAdd.addr[5];

        return 0;
    }

    MAC_ADDR getMacAddress(){
        return _macAdd;
    }

    int setCallbackFun(AOParseFunction callbackFun){
        _callbackFun = callbackFun;
        return 0;
    }

    static int unpackData(short *pDataOut, int *dataSize, int dataCaptured, short *pDataIn){
        int idx(0);
        int cDataTemp(0);

        while (idx < dataCaptured) {
                for (int i(0); i<pDataIn[idx]-7; i++) {
                        pDataOut[cDataTemp] = pDataIn[i+7+idx];
                        cDataTemp++;
                }
                idx = idx + pDataIn[idx];
        }

        *dataSize = cDataTemp;

        return 0;
    }

    virtual int connect() = 0;
    virtual int isConnected() = 0;
    virtual int disconnect() = 0;

    virtual int addBufferChannel(long channelId, int bufferSizemSec) = 0;
    virtual int getChannelData(int channelId, short *pData, int arrSizeWords,int *dataCaptured) = 0;
    virtual int clearBuffers() = 0;

    virtual int addRecordingChannel(int channelID) = 0;
    virtual int startRecording() = 0;
    virtual int stopRecording() = 0;
    
    virtual int setStimChan(vector<StimChan> chan, bool ExclusiveMode=false) = 0;

    virtual int setStimParm(int idx, StimParm stimParm) = 0;
    virtual int startStim(int idx, double duration_sec) = 0;
    virtual int stopStim(int idx) = 0;
    virtual int stopStim() = 0;

    virtual void startStimRamp(int idx, StimParm stimParm, double duration_sec) = 0;
    virtual void stopStimRamp(int idx) = 0;
    virtual void stopStimRamp() = 0;

protected:
    MAC_ADDR _macAdd;
    AOParseFunction _callbackFun;

    bool _exclusiveStim;
};

#endif // STIMDEVICE_H
