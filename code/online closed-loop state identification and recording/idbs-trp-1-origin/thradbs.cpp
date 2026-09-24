#include "thradbs.h"
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QDebug>

THRaDBS::THRaDBS(QString configFile)
{    
    QSettings config(configFile, QSettings::IniFormat);

    _amplitude = config.value("parameter/amplitude").toDouble();
    _pulseWidth = config.value("parameter/pulsewidth").toDouble();
    _freq = config.value("parameter/frequency").toInt();

    _threshold = config.value("parameter/threshold").toDouble();

    _nInit = config.value("parameter/ninit").toInt();
    _pBufferInit = new double[_nInit];
    _isInit = true;
    _idxInit = 0;

    _winSize = config.value("parameter/winsize").toInt();
    _pBuffer = new double[_winSize]{0};

    QString sosMatStr = config.value("parameter/sosmat").toString();
    QStringList sosMatLine = sosMatStr.split(';',QString::SkipEmptyParts);

    _nSOS = sosMatLine.count();
    _sosMat = new double *[_nSOS];
    for(int i=0;i<_nSOS;i++){
        _sosMat[i] = new double[6];
        QStringList sosMatValue = sosMatLine.at(i).split(',',QString::SkipEmptyParts);
        for(int j=0;j<6;j++){
            _sosMat[i][j] = sosMatValue.at(j).toDouble();
            qDebug() <<  _sosMat[i][j];
        }
    }

    _filter = new CascadeIIR(_nSOS,_sosMat);

    QDateTime datetime;
    QString timestr = datetime.currentDateTime().toString("yyyyMMddHHmmss");
    QString outpath = config.value("parameter/savepath").toString();

    QDir dir;
    if (!dir.exists(outpath))
    {
        dir.mkpath(outpath);
    }

    QString filename = outpath+timestr+".txt";
    _file = new QFile(filename);
    if(!_file->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << filename;
    _stream = new QTextStream(_file);


    *_stream << "clean\tbeta\tstim\tthr" << endl;

}

THRaDBS::~THRaDBS(){
    if(_filter){
        delete _filter;
    }

    for(int i=0;i<_nSOS;i++){
        if(_sosMat[i]){
            delete [] _sosMat[i];
        }
    }

    if(_sosMat){
        delete [] _sosMat;
    }

    if(_pBuffer){
        delete [] _pBuffer;
    }

    if(_pBufferInit){
        delete [] _pBufferInit;
    }

    _stream->flush();
    delete _stream;
    _file->close();
    _file->deleteLater();
}

void THRaDBS::recieveData(double sample){
    double filtered = _filter->filter(sample);
    shiftBuffer(fabs(filtered));
    double mean(double *pData, int N);
    double ampl = mean(_pBuffer,_winSize);

    *_stream << filtered << "\t" << ampl;

    if(_isInit){
        _pBufferInit[_idxInit] = ampl;
        _idxInit++;
        if(_idxInit>=_nInit){
            double getThreshold(double *pData, int N, double thr);
            _threshold = getThreshold(_pBufferInit,_nInit,_threshold);
            *_stream << "\t" << _threshold;
            _isInit = false;
        }
    }else{
        if(ampl>_threshold){
            emit signalStimulation(_amplitude,_pulseWidth,_freq,true);
            *_stream << "\t" << 1;
        }else{
            emit signalStimulation(0,0,0,false);
            *_stream << "\t" << 0;
        }
    }

    *_stream << endl;
}

void THRaDBS::shiftBuffer(double sample){
    for(int i=0;i<_winSize-1;i++){
        _pBuffer[i] = _pBuffer[i+1];
    }
    _pBuffer[_winSize-1] = sample;
}

double getThreshold(double *pData, int N, double thr){
    std::sort(pData,pData+N);

    int idx = floor(N*thr);

    return pData[idx];
}


double mean(double *pData, int N){
    double sum = 0;
    for(int i=0;i<N;i++){
        sum = sum + pData[i];
    }

    return sum/N;
}

