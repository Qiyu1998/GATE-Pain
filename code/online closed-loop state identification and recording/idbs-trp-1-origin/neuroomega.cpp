#include "neuroomega.h"
#include <QDebug>

NeuroOmega::NeuroOmega()
{
    _lib = new QLibrary("NeuroOmega.dll");
    if (_lib->load()) {
        qDebug() << "NeuroOmega.dll loaded successfully." << endl;
    } else {
        qDebug() << "Failed to load NeuroOmega.dll!" << endl;
    }
}

void NeuroOmega::displayError(){
    //DECLDIR int ErrorHandlingfunc(int* pErrorCount, cChar* sError, int nError);
    typedef int (*FUN)(int* pErrorCount, char* sError, int nError);
    FUN geterr = (FUN) _lib->resolve("ErrorHandlingfunc");
    int errCount = 0;
    char sErr[100];
    int nErr = 100;
    geterr(&errCount, sErr, nErr);
    qDebug() << endl << sErr << endl;
}

int NeuroOmega::connect()
{
    //DECLDIR int DefaultStartConnection(MAC_ADDR* pSystemMAC, AOParseFunction callback=NULL);
    typedef int (*FUN)(MAC_ADDR* pSystemMAC, AOParseFunction callback);
    FUN DefaultStartConnection = (FUN) _lib->resolve("DefaultStartConnection");

    int ret = 0;
    if (DefaultStartConnection){
        ret = DefaultStartConnection(&_macAdd, nullptr);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::isConnected()
{
    //DECLDIR	int isConnected();
    typedef int (*FUN)();
    FUN aoIsConnected = (FUN) _lib->resolve("isConnected");

    if (aoIsConnected)
        return aoIsConnected();
    else
        return -1;
}

int NeuroOmega::disconnect()
{
    //DECLDIR	int CloseConnection();
    typedef int (*FUN)();
    FUN CloseConnection = (FUN) _lib->resolve("CloseConnection");

    int ret = 0;
    if (CloseConnection){
        ret = CloseConnection();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::addBufferChannel(long channelId, int bufferSizemSec)
{
    //DECLDIR int AddBufferChannel(int nChannelID, int nBufferingTime_mSec);
    typedef int (*FUN)(int nChannelID, int nBufferingTime_mSec);
    FUN AddBufferChannel = (FUN) _lib->resolve("AddBufferChannel");

    int ret = 0;
    if (AddBufferChannel){
        ret = AddBufferChannel(channelId, bufferSizemSec);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::getChannelData(int channelId, short *pData, int arrSizeWords,int *dataCaptured)
{
    //DECLDIR int GetChannelData(int nChannelID, int16* pData, int nData, int* pDataCapture);
    typedef int (*FUN)(int nChannelID, short* pData, int nData, int* pDataCapture);
    FUN GetChannelData = (FUN) _lib->resolve("GetChannelData");

    int ret = 0;
    if (GetChannelData){
        ret = GetChannelData(channelId, pData, arrSizeWords, dataCaptured);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::clearBuffers()
{
    //DECLDIR int ClearBuffers();
    typedef int (*FUN)();
    FUN ClearBuffers = (FUN) _lib->resolve("ClearBuffers");

    int ret = 0;
    if (ClearBuffers){
        ret = ClearBuffers();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::addRecordingChannel(int channelID)
{
    //DECLDIR int SetChannelSaveState(int nChannelID, BOOL bSave);
    typedef int (*FUN)(int nChannelID, bool bSave);
    FUN SetChannelSaveState = (FUN) _lib->resolve("SetChannelSaveState");

    int ret = 0;
    if (SetChannelSaveState){
        ret = SetChannelSaveState(channelID,true);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::startRecording()
{
    //DECLDIR	int StartSave();
    typedef int (*FUN)();
    FUN StartSave = (FUN) _lib->resolve("StartSave");

    int ret = 0;
    if (StartSave){
        ret = StartSave();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::stopRecording()
{
    //DECLDIR	int StopSave();
    typedef int (*FUN)();
    FUN StopSave = (FUN) _lib->resolve("StopSave");

    int ret = 0;
    if (StopSave){
        ret = StopSave();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int NeuroOmega::setStimChan(vector<StimChan> chan, bool ExclusiveMode){
    _stimChan.clear();
    for (int i(0); i<chan.size(); i++) {
        StimChanConfig *temp = new StimChanConfig(this, chan.at(i), i);
        _stimChan.push_back(temp);
    }

    _exclusiveStim = ExclusiveMode;

    if (ExclusiveMode){
        StimParm stimParm;
        stimParm.freq_Hz = 130;
        stimParm.amplitude_mA = 0.001;
        stimParm.pulseWidth_mS = 0.06;

        int ret = 0;
        for (int i(0); i<chan.size(); i++) {
            ret = setStimParm(i, stimParm);
            if (ret!=0){
                return -1;
            }
            ret = startStim(i, STIM_DURATION);
            if (ret!=0){
                return -1;
            }
        }
    }

    return 0;
}

int NeuroOmega::setStimParm(int idx, StimParm stimParm){ 
    //DECLDIR	int SetStimulationParameters(real32 fFirstPhaseAmp_mA, real32 fFirstPhaseWidth_mS, \
    //                                        real32 fSecondPhaseAmp_mA, real32 fSecondPhaseWidth_mS, int nFreq_Hz,\
    //                                         real32 fDuration_Sec, int nReturnChannel, int nContactID, \
    //                                          real32 fFirstPhaseDelay_mS=0, real32 fSecondPhaseDelay_mS=0);
    typedef int (*FUN)(float FirstPhaseAmpl_mA,float FirstPhaseWidth_mS, \
                        float fSecondPhaseAmp_mA, float fSecondPhaseWidth_mS, int nFreq_Hz, \
                         float fDuration_Sec, int nReturnChannel, int nContactID, \
                          float fFirstPhaseDelay_mS, float fSecondPhaseDelay_mS);
    FUN SetStimulationParameters = (FUN) _lib->resolve("SetStimulationParameters");

    int channelID = _stimChan.at(idx)->getChan().stimChanID;
    int returnChannel = _stimChan.at(idx)->getChan().returnChanID;
    double firstPhaseAmpl_mA = stimParm.amplitude_mA;
    double firstPhaseWidth_mS = stimParm.pulseWidth_mS;
    double secondPhaseAmpl_mA = -firstPhaseAmpl_mA/BALANCE_PULSE_TIME;
    double secondPhaseWidth_mS = firstPhaseWidth_mS*BALANCE_PULSE_TIME;
    double firstPhaseDelay_mS = 0;
    double secondPhaseDelay_mS = 0;
    int freq = stimParm.freq_Hz;

    int ret = SetStimulationParameters(firstPhaseAmpl_mA, firstPhaseWidth_mS, \
                                        secondPhaseAmpl_mA, secondPhaseWidth_mS, freq, \
                                            STIM_DURATION, returnChannel, channelID, \
                                                firstPhaseDelay_mS, secondPhaseDelay_mS);
    if (ret != 0){
        displayError();
        return -1;
    }

    _stimChan.at(idx)->setParm(stimParm);

    return 0;
}

int NeuroOmega::startStim(int idx, double duration_sec){
    // DECLDIR	int StartStimulation(int nChannelID);
    typedef int (*FUN)(int nChannelID);
    FUN StartStimulation = (FUN) _lib->resolve("StartStimulation");

    int ret = StartStimulation(_stimChan.at(idx)->getChan().stimChanID);
    if (ret != 0){
        displayError();
        return -1;
    }

    _stimChan.at(idx)->stimStarted(duration_sec);

    return 0;
}

int NeuroOmega::stopStim(int idx){
    //DECLDIR	int StopStimulation(int ChannelID);
    typedef int (*FUN)(int ChannelID);
    FUN StopStimulation = (FUN) _lib->resolve("StopStimulation");

    if (_exclusiveStim){
        StimParm stimParm;
        stimParm.freq_Hz = 130;
        stimParm.amplitude_mA = 0.001;
        stimParm.pulseWidth_mS = 0.06;

        int ret = setStimParm(idx, stimParm);
        if (ret!=0){
            displayError();
            return -1;
        }
        ret = startStim(idx, STIM_DURATION);
        if (ret!=0){
            displayError();
            return -1;
        }

        _stimChan.at(idx)->stimStoped();

        return 0;
    }else {
        int ret = StopStimulation(_stimChan.at(idx)->getChan().stimChanID);
        if (ret!=0){
            displayError();
            return -1;
        }
        _stimChan.at(idx)->stimStoped();
        return 0;
    }
    _stimChan.at(idx)->stimStoped();
    return 0;
}

int NeuroOmega::stopStim(){
    int ret;
    for (int i(0); i<_stimChan.size(); i++) {
        if(_stimChan.at(i)->getStimStatus()){
            ret = stopStim(i);
            if(ret!=0){
                displayError();
                return -1;
            }
        }
    }
    return 0;
}

void NeuroOmega::startStimRamp(int idx, StimParm stimParm, double duration_sec){
    _stimChan.at(idx)->stimStartRamp(stimParm);
    _stimChan.at(idx)->rampStimStarted(duration_sec);
}

void NeuroOmega::stopStimRamp(int idx){
    _stimChan.at(idx)->stimStopRamp();
    _stimChan.at(idx)->rampStimStoped();
}

void NeuroOmega::stopStimRamp(){
    for (int i(0); i<_stimChan.size(); i++) {
        if(_stimChan.at(i)->getStimStatus()){
            stopStimRamp(i);
        }
    }
}

NeuroOmega::~NeuroOmega(){
    _lib->deleteLater();
    for (int i(0); i<_stimChan.size(); i++) {
        _stimChan.at(i)->deleteLater();
    }
    _stimChan.clear();
}


