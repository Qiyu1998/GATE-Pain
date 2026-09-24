#include "alphalabsnr.h"
#include <QDebug>

AlphaLabSNR::AlphaLabSNR()
{
    _lib = new QLibrary("AlphaLabSNR.dll");
    if (_lib->load()) {
        qDebug() << "AlphaLabSNR.dll loaded successfully." << endl;
    } else {
        qDebug() << "Failed to load AlphaLabSNR.dll!" << endl;
    }
}

AlphaLabSNR::~AlphaLabSNR(){
    _lib->deleteLater();
    for (int i(0); i<_stimChan.size(); i++) {
        _stimChan.at(i)->deleteLater();
        _stimChan.at(i) = nullptr;
    }
    _stimChan.clear();
}

void AlphaLabSNR::displayError(){
    //DECLDIR int AO_GetError(int* pErrorCount, cChar* sError, int nError);
    typedef int (*FUN)(int* pErrorCount, char* sError, int nError);
    FUN geterr = (FUN) _lib->resolve("AO_GetError");
    int errCount = 0;
    char sErr[100];
    int nErr = 100;
    geterr(&errCount, sErr, nErr);
    qDebug() << endl << sErr << endl;
}

int AlphaLabSNR::connect()
{
    //DECLDIR int AO_DefaultStartConnection(MAC_ADDR* pSystemMAC, AOParseFunction callback=NULL);
    //[IN] See AOParseFunction. This input parameter is OBSOLETE and should be NULL.
    typedef int (*FUN)(MAC_ADDR* pSystemMAC, AOParseFunction callback);
    FUN StartConnection = (FUN) _lib->resolve("AO_DefaultStartConnection");

    int ret = 0;
    if (StartConnection){
        ret = StartConnection(&_macAdd, nullptr);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::isConnected()
{
    //DECLDIR	int AO_IsConnected();
    typedef int (*FUN)();
    FUN aoIsConnected = (FUN) _lib->resolve("AO_IsConnected");

    if (aoIsConnected){
        return aoIsConnected();
    }
    else
        return -1;
}

int AlphaLabSNR::disconnect()
{
    //DECLDIR	int AO_CloseConnection();
    typedef int (*FUN)();
    FUN CloseConnection = (FUN) _lib->resolve("AO_CloseConnection");

    int ret = 0;
    if (CloseConnection){
        ret = CloseConnection();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::addBufferChannel(long channelId, int bufferSizemSec)
{
    //DECLDIR int AO_AddBufferChannel(int nChannelID, int nBufferingTime_mSec);
    typedef int (*FUN)(int nChannelID, int nBufferingTime_mSec);
    FUN AddBufferChannel = (FUN) _lib->resolve("AO_AddBufferChannel");

    int ret = 0;
    if (AddBufferChannel){
        ret = AddBufferChannel(channelId, bufferSizemSec);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::getChannelData(int channelId, short *pData, int arrSizeWords,int *dataCaptured)
{
    //DECLDIR int AO_GetChannelData(int nChannelID, int16* pData, int nData, int* pDataCapture);
    typedef int (*FUN)(int nChannelID, short* pData, int nData, int* pDataCapture);
    FUN GetChannelData = (FUN) _lib->resolve("AO_GetChannelData");

    int ret = 0;
    if (GetChannelData){
        ret = GetChannelData(channelId, pData, arrSizeWords, dataCaptured);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::clearBuffers()
{
    //DECLDIR int AO_ClearBuffers();
    typedef int (*FUN)();
    FUN ClearBuffers = (FUN) _lib->resolve("AO_ClearBuffers");

    int ret = 0;
    if (ClearBuffers){
        ret = ClearBuffers();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::addRecordingChannel(int channelID)
{
    //DECLDIR int AO_SetChannelSaveState(int nChannelID, BOOL bSave);
    typedef int (*FUN)(int nChannelID, bool bSave);
    FUN SetChannelSaveState = (FUN) _lib->resolve("AO_SetChannelSaveState");

    int ret = 0;
    if (SetChannelSaveState){
        ret = SetChannelSaveState(channelID,true);
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::startRecording()
{
    //DECLDIR	int AO_StartSave();
    typedef int (*FUN)();
    FUN StartSave = (FUN) _lib->resolve("AO_StartSave");

    int ret = 0;
    if (StartSave){
        ret = StartSave();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::stopRecording()
{
    //DECLDIR	int AO_StopSave();
    typedef int (*FUN)();
    FUN StopSave = (FUN) _lib->resolve("AO_StopSave");

    int ret = 0;
    if (StopSave){
        ret = StopSave();
        if (ret!=0){
            displayError();
        }
    }

    return ret;
}

int AlphaLabSNR::setStimChan(vector<StimChan> chan, bool ExclusiveMode){
    _stimChan.clear();
    for (int i(0); i<chan.size(); i++) {
        StimChanConfig *temp = new StimChanConfig(this, chan.at(i), i);
        _stimChan.push_back(temp);
    }

    _exclusiveStim = ExclusiveMode;

    if (ExclusiveMode){
        StimParm stimParm;
        stimParm.freq_Hz = 130;
        stimParm.amplitude_mA = 0.0004;
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

int AlphaLabSNR::setStimParm(int idx, StimParm stimParm){
    //DECLDIR	int AO_SetStimulationParameters(real32 fFirstPhaseAmp_mA, real32 fFirstPhaseWidth_mS, \
    //                                              real32 fSecondPhaseAmp_mA, real32 fSecondPhaseWidth_mS, int nFreq_Hz,\
    //                                                  real32 fDuration_Sec, int nReturnChannel, int nContactID, real32 fFirstPhaseDelay_mS=0, real32 fSecondPhaseDelay_mS=0);

    typedef int (*FUN)(float fFirstPhaseAmp_mA, float fFirstPhaseWidth_mS, \
                        float fSecondPhaseAmp_mA, float fSecondPhaseWidth_mS, int nFreq_Hz, \
                         float fDuration_Sec, int nReturnChannel, int nContactID, float fFirstPhaseDelay_mS, float fSecondPhaseDelay_mS);
    FUN SetStimualtionParameters = (FUN) _lib->resolve("AO_SetStimulationParameters");

    int channelID = _stimChan.at(idx)->getChan().stimChanID;
    int returnChannel = _stimChan.at(idx)->getChan().returnChanID;
    double firstPhaseAmpl_mA = stimParm.amplitude_mA;
    double firstPhaseWidth_mS = stimParm.pulseWidth_mS;
    double secondPhaseAmpl_mA = -firstPhaseAmpl_mA/BALANCE_PULSE_TIME;
    double secondPhaseWidth_mS = firstPhaseWidth_mS*BALANCE_PULSE_TIME;
    int freq = stimParm.freq_Hz;

    int ret = SetStimualtionParameters(firstPhaseAmpl_mA, firstPhaseWidth_mS, \
                                        secondPhaseAmpl_mA, secondPhaseWidth_mS, freq, \
                                            STIM_DURATION, returnChannel, channelID, 0, 0);
    if (ret != 0){
        displayError();
        return -1;
    }

    _stimChan.at(idx)->setParm(stimParm);

    return 0;
}

int AlphaLabSNR::startStim(int idx, double duration_sec){
    //DECLDIR	int AO_StartStimulation(int nContactID);
    typedef int (*FUN2)(int nContactID);
    FUN2 StartStimulation = (FUN2) _lib->resolve("AO_StartStimulation");

    int ret = StartStimulation(_stimChan.at(idx)->getChan().stimChanID);
    if (ret != 0){
        displayError();
        return -1;
    }

    _stimChan.at(idx)->stimStarted(duration_sec);

    return 0;
}

int AlphaLabSNR::stopStim(int idx)
{
    //DECLDIR	int AO_StopStimulation(int nContactID);
    typedef int (*FUN)(int nContactID);
    FUN StopStimulation = (FUN) _lib->resolve("AO_StopStimulation");

    if (_exclusiveStim){
        StimParm stimParm;
        stimParm.freq_Hz = 130;
        stimParm.amplitude_mA = 0.0004;
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

int AlphaLabSNR::stopStim()
{
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

void AlphaLabSNR::startStimRamp(int idx, StimParm stimParm, double duration_sec){
    _stimChan.at(idx)->stimStartRamp(stimParm);
    _stimChan.at(idx)->rampStimStarted(duration_sec);
}

void AlphaLabSNR::stopStimRamp(int idx){
    _stimChan.at(idx)->stimStopRamp();
    _stimChan.at(idx)->rampStimStoped();
}

void AlphaLabSNR::stopStimRamp(){
    for (int i(0); i<_stimChan.size(); i++) {
        if(_stimChan.at(i)->getStimStatus()){
            stopStimRamp(i);
        }
    }
}



