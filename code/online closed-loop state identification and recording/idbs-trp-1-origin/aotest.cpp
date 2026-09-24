#include "aotest.h"
#include <QDebug>

AOTest::AOTest()
{
    _isConnected = 0;
    _samplingRate = 44000;
    qsrand(time(nullptr));

    qDebug() << "AOTest..." << endl;
}

AOTest::~AOTest(){
    if(_tGenData){
        if(_tGenData->isActive()){
            _tGenData->stop();
        }
        _tGenData->deleteLater();
        _tGenData = nullptr;
    }

    if(_pData){
        delete [] _pData;
        _pData = nullptr;
    }
}

int AOTest::connect()
{
    _isConnected = 1;
    return 0;
}

int AOTest::isConnected()
{
    return _isConnected;
}

int AOTest::disconnect()
{
    _isConnected = 0;

    QIODevice::disconnect(_tGenData, &QTimer::timeout, this, &AOTest::genData);
    _tGenData->deleteLater();
    _tGenData = nullptr;

    return 0;
}

int AOTest::addBufferChannel(long channelId, int bufferSizemSec)
{
    _bufferSizePoints = bufferSizemSec*_samplingRate/1000;

    _tGenData = new QTimer(this);
    _tGenData->setInterval(10);

    _pData = new short[_bufferSizePoints+7];
    _pData[0] = 7;
    _pData[2]  = channelId;

    QIODevice::connect(_tGenData, &QTimer::timeout, this, &AOTest::genData);
    _tGenData->start();

    return 0;
}

void AOTest::genData()
{
    for(int i=0;i<440;i++){
        int idx = _pData[0];
        if(idx<=_bufferSizePoints){
            short n = qrand() % 200;
            _pData[idx] = n-100;
            _pData[0] += 1;
        }
    }
}

int AOTest::getChannelData(int channelId, short *pData, int arrSizeWords,int *dataCaptured)
{
    if (_pData[2]==channelId) {
        if (arrSizeWords>_pData[0]) {
            *dataCaptured = _pData[0]-7;

            for (int i(0); i<_pData[0]; i++) {
                pData[i] = _pData[i];
            }
            this->clearBuffers();
            return 0;
        } else {
            return 7;
        }
    } else {
        return 12;
    }
}

int AOTest::clearBuffers()
{
    _pData[0] = 7;
    return 0;
}

int AOTest::addRecordingChannel(int channelID)
{
    channelID++;
    return 0;
}

int AOTest::startRecording()
{
    return 0;
}

int AOTest::stopRecording()
{
    return 0;
}

int AOTest::setStimChan(vector<StimChan> chan, bool ExclusiveMode){
    Q_UNUSED(chan);
    Q_UNUSED(ExclusiveMode);
    return 0;
}

int AOTest::setStimParm(int idx, StimParm stimParm){
    Q_UNUSED(idx);
    Q_UNUSED(stimParm);
    return 0;
}

int AOTest::startStim(int idx, double duration_sec){
    Q_UNUSED(idx);
    Q_UNUSED(duration_sec);
    return 0;
}

int AOTest::stopStim(int idx){
    Q_UNUSED(idx);
    return 0;
}

int AOTest::stopStim(){
    return 0;
}

void AOTest::startStimRamp(int idx, StimParm stimParm, double duration_sec){
    Q_UNUSED(idx);
    Q_UNUSED(stimParm);
    Q_UNUSED(duration_sec);
}

void AOTest::stopStimRamp(int idx){
    Q_UNUSED(idx);
}

void AOTest::stopStimRamp(){
}

void AOTest::getSamplingRateList(int *pSRList){
    pSRList[0] = 275;
    return;
}

int AOTest::getSamplingRateNumber(){
   return 1;
}
