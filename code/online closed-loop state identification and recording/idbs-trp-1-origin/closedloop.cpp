#include "closedloop.h"
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <stdio.h>
#include <QDebug>

const int BUFFER_SIZE_MS = 10000;
const int DISPfs = 275;
const int N_DISP = 25;
const int SAVE_MS = 200;

ClosedLoop::ClosedLoop(QObject *parent) : QObject(parent)
{
}

ClosedLoop::~ClosedLoop(){
    close();
}

void ClosedLoop::init(StimStrategyList cDBS, QString configFile){
    QSettings config(configFile, QSettings::IniFormat);

    string deviceName = config.value("system/device").toString().toStdString();
    qDebug() << QString::fromStdString(deviceName) << endl;
    if(!DeviceFactory::isValid(deviceName)){
        emit dispMsg("Invalid config file: unkonwn device.");
        qDebug() << "Invalid config file: unkonwn device." << endl;
        return;
    }
    StimDeviceList ao = DeviceFactory::strToEnum(deviceName);

    QString macStr = config.value("system/mac").toString();
    qDebug() << macStr << endl;
    MAC_ADDR mac;
    QStringList spl = macStr.split(':', QString::SkipEmptyParts);
    if(spl.count()!=6){
        emit dispMsg("Invalid config file: invalid mac address.");
        qDebug() << "Invalid config file: invalid mac address." << endl;
        return;
    }
    for(int i=0;i<6;i++){
        mac.addr[i] = spl.at(i).toInt(Q_NULLPTR,16);
    }

    int recChnID = config.value("recording/channel").toInt();
    int fsraw = config.value("recording/fs").toInt();
    int stimChnID = config.value("stimulation/stimchan").toInt();
    int retnChnID = config.value("stimulation/retnchan").toInt();

    emit systemStateChanged("connecting");

    // Init cDBS method
    _isStim = false;
    _cDBS = StrategyFactory::getStimSuper(cDBS, configFile);
    connect(_cDBS, &StimStrategy::signalStimulation, this, &ClosedLoop::doStim);

    // Init CleanDBS
    _enableCleanDBS = config.value("cleandbs/enable").toBool();
    double fsCleanDBS;
    if(_enableCleanDBS){
        double threshold = config.value("cleandbs/threshold").toDouble();
        double precut = config.value("cleandbs/precut").toDouble();
        double postcut = config.value("cleandbs/postcut").toDouble();
        fsCleanDBS = config.value("cleandbs/downsample").toDouble();

        _cleanDBS = new CleanDBS(fsraw,fsCleanDBS,precut,postcut,threshold);
    }

    // Init Filter
    _enableFilter = config.value("filter/enable").toBool();
    int fscdbs = config.value("filter/downsample").toInt();
    qDebug() << "fscdbs" << fscdbs << endl;
    _enableResample = false;

    if(_enableFilter){
        QString sosMatStr = config.value("filter/sosmat").toString();
        qDebug() << ".....!!!!" << sosMatStr;
        QStringList sosMatLine = sosMatStr.split(';',QString::SkipEmptyParts);

        _nSOS = sosMatLine.count();
        _sosMat = new double *[_nSOS];
        for(int i=0;i<_nSOS;i++){
            _sosMat[i] = new double[6];
            QStringList sosMatValue = sosMatLine.at(i).split(',',QString::SkipEmptyParts);
            for(int j=0;j<6;j++){
                _sosMat[i][j] = sosMatValue.at(j).toDouble();
            }
        }

        _filter = new CascadeIIR(_nSOS,_sosMat);

        if(fscdbs>0){
            _enableResample = true;

            _rsCDBS = new Resampling();

            if(_enableCleanDBS){
                _rsCDBS->setSamplingRate(fsCleanDBS, fscdbs);
            }else{
                _rsCDBS->setSamplingRate(fsraw, fscdbs);
            }
        }
    }


    _rsChart = new Resampling();
    _rsChart->setSamplingRate(fsraw, DISPfs);


    // Init stimulator
    _stimulator = DeviceFactory::getAlphaOmegaDevice(ao);
    _stimulator->setMacAddress(mac);

    // Init buffer
    _bufferSize  = BUFFER_SIZE_MS*fsraw/1000+7;
    _pBuffer = new short[_bufferSize];

    _pDispBuffer = new double[N_DISP];
    _idxDisp = 0;

    // Init saving
    _enableSaving = config.value("recording/save").toBool();

    if(_enableSaving){
        // Init buffer for saving
        _saveSize = fsraw*SAVE_MS/1000;
        _pSaveBuffer = new short[_saveSize];
        _idxSave = 0;

        // Create folder
        QString outpath = config.value("recording/savepath").toString();
        QDir dir;
        if (!dir.exists(outpath))
        {
            dir.mkpath(outpath);
        }

        // Init saving stream
        QDateTime datetime;
        QString timestr = datetime.currentDateTime().toString("yyyyMMddHHmmss");
        QString fileName;

        fileName = outpath + "RAW-" + timestr + ".txt";
        _pFileRaw = new QFile(fileName);
        if(!_pFileRaw->open(QIODevice::ReadWrite|QIODevice::Text))
        {
            emit dispMsg("Cannot open file.");
            return;
        }

        _pStreamRaw = new QTextStream(_pFileRaw);
    }

    // Connect
    qDebug() << "Connecting..." << endl;
    _stimulator->connect();
    QTime tConnect;
    tConnect.start();

    while (tConnect.elapsed()<1000) {
        continue;
    }

    if (!_stimulator->isConnected()) {
        emit systemStateChanged("disconnected");
        qDebug()<<"connect failed..."<<endl;
        emit dispMsg(QString("connect failed"));
        return;
    }

    _recChnID = recChnID;
    _stimChan.stimChanID = stimChnID;
    _stimChan.returnChanID = retnChnID;

    vector<StimChan> temp;
    temp.push_back(_stimChan);
    int ret = _stimulator->setStimChan(temp, false); //delicate
    if(ret!=0){
        emit systemStateChanged("disconnected");
        emit dispMsg(QString("set stim failed"));
        return;
    }

    ret = _stimulator->addBufferChannel(recChnID, BUFFER_SIZE_MS);
    if(ret!=0){
        emit systemStateChanged("disconnected");
        emit dispMsg(QString("add buffer failed"));
        return;
    }

    // Init timer
    _tGetData = new QTimer(this);
    connect(_tGetData, &QTimer::timeout, this, &ClosedLoop::getData);
    _tGetData->setInterval(10);
    _tGetData->start();

    emit systemStateChanged("connected");
}

void ClosedLoop::close(){
    _isStim = false;

    if(_stimulator){
        _stimulator->stopStim();
        _stimulator->deleteLater();
        _stimulator = nullptr;
    }

    disconnect(_cDBS, &StimStrategy::signalStimulation, this, &ClosedLoop::doStim);
    disconnect(_tGetData, &QTimer::timeout, this, &ClosedLoop::getData);

    if(_pStreamRaw){
        _pStreamRaw->flush();
        delete _pStreamRaw;
        _pStreamRaw = nullptr;
    }

    if(_pFileRaw){
        _pFileRaw->close();
        _pFileRaw->deleteLater();
        _pFileRaw = nullptr;
    }

    if(_cDBS){
        _cDBS->deleteLater();
        _cDBS = nullptr;
    }

    if(_tGetData){
        if(_tGetData->isActive()){
            _tGetData->stop();
        }
        _tGetData->deleteLater();
        _tGetData = nullptr;
    }

    if(_pBuffer){
        delete [] _pBuffer;
        _pBuffer = nullptr;
    }

    if(_pDispBuffer){
        delete [] _pDispBuffer;
        _pDispBuffer = nullptr;
    }

    if(_pSaveBuffer){
        delete [] _pSaveBuffer;
        _pSaveBuffer = nullptr;
    }

    if(_rsCDBS){
        delete _rsCDBS;
        _rsCDBS = nullptr;
    }

    if(_rsChart){
        delete _rsChart;
        _rsChart = nullptr;
    }

    for(int i=0;i<_nSOS;i++){
        if(_sosMat[i]){
            delete [] _sosMat[i];
        }
    }
    if(_sosMat){
        delete [] _sosMat;
    }

    if(_filter){
        delete _filter;
    }

    emit systemStateChanged("disconnected");
}

void ClosedLoop::startCDBS(){
    if(_stimulator->isConnected()){
        _isStim = true;
        emit systemStateChanged("startstim");
    }
}

void ClosedLoop::stopCDBS(){
    _isStim = false;
    _stimulator->stopStim();
    emit systemStateChanged("stopstim");
}

void ClosedLoop::getData(){

    int dataCapture = 0;
    int ret = _stimulator->getChannelData(_recChnID, _pBuffer, _bufferSize, &dataCapture);
    ret;
    if (ret==0 && dataCapture) {
        short *pDataUnpacked = new short[dataCapture];
        int dataSize = 0;

        StimDevice::unpackData(pDataUnpacked, &dataSize, dataCapture, _pBuffer);

        for(int i=0;i<dataSize;i++){
            // cDBS
            if(_isStim){
                if(_enableCleanDBS){
                    double fData;
                    if(_cleanDBS->recieve(pDataUnpacked[i],&fData)){
                        if(_enableFilter){
                            // CleanDBS - ON    Filter - ON
                            fData = _filter->filter(fData);
                        }   // CleanDBS - ON    Filter - OFF
                        _cDBS->recieveData(fData);
                    }
                }else{
                    double fData;
                    if(_enableFilter){
                        // CleanDBS - OFF    Filter - ON
                        fData = _filter->filter(pDataUnpacked[i]);
                    }else{
                        // CleanDBS - OFF    Filter -OFF
                        fData = pDataUnpacked[i];
                    }

                    if(_enableResample){
                        // Enable resample
                        double temp1;
                        if(_rsCDBS->downsampling(fData, &temp1)){
                            _cDBS->recieveData(temp1);
                        }
                    }else{
                        // Disable resample
                        _cDBS->recieveData(fData);
                    }
                }

                // Save raw data
                if(_enableSaving){
                    _pSaveBuffer[_idxSave] = pDataUnpacked[i];
                    _idxSave++;
                    if(_idxSave>=_saveSize){
                        savedata();
                        _idxSave = 0;
                    }
                }
            }

            // Resample for display
            double temp2;
            if(_rsChart->downsampling(pDataUnpacked[i], &temp2)){
                _pDispBuffer[_idxDisp] = temp2;
                _idxDisp++;
                if(_idxDisp>=N_DISP){
                    double *pTemp = new double[N_DISP];
                    memcpy(pTemp, _pDispBuffer, N_DISP*sizeof(double));

                    // YOU MUST DELECT pTemp IN THE SOLT FUNCTION !!!
                    emit updateCharts(pTemp,N_DISP);

                    _idxDisp = 0;
                }
            }
        }

        delete[] pDataUnpacked;
        pDataUnpacked = nullptr;
    }

    return;
}

void ClosedLoop::doStim(double amplitude, double pulseWidth, int freq, bool stimOnOff){
    if(stimOnOff){
        StimParm stim;
        stim.amplitude_mA = amplitude;
        stim.pulseWidth_mS = pulseWidth;
        stim.freq_Hz = freq;
        _stimulator->setStimParm(0,stim);
        _stimulator->startStim(0,STIM_DURATION);
        //_stimulator->startStimRamp(0,stim,STIM_DURATION);
    }else{
        _stimulator->stopStim(0);
        //_stimulator->stopStimRamp();
    }
    emit stimStateChanged(amplitude, pulseWidth, freq, stimOnOff);
}

void ClosedLoop::savedata(){
    for(int i=0;i<_saveSize;i++){
        *_pStreamRaw << _pSaveBuffer[i] << endl;
    }
    _pStreamRaw->flush();
}
