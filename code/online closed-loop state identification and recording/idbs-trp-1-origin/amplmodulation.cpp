#include "amplmodulation.h"
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <algorithm>
#include "lib/kissfft/kiss_fftr.h"

using namespace std;

AmplModulation::AmplModulation(QString configFile)
{
    QSettings config(configFile, QSettings::IniFormat);

    _fs = config.value("parameter/fs").toInt();

    // Frequency band of target oscillation
    QString freqband = config.value("parameter/freqband").toString();
    QStringList freqbandValue = freqband.split(',',QString::SkipEmptyParts);
    _freqband[0] = freqbandValue.at(0).toDouble();
    _freqband[1] = freqbandValue.at(1).toDouble();

    // Initial stimulation parameters
    _amplitude = config.value("parameter/amplitude").toDouble();
    _pulseWidth = config.value("parameter/pulsewidth").toDouble();
    _freq = config.value("parameter/frequency").toInt();

    // Stimulation amplitude limitations
    _amplLimMax = config.value("parameter/amplmax").toDouble();
    _amplLimMin = config.value("parameter/amplmin").toDouble();
    _maxRate = config.value("parameter/maxrate").toDouble();

    // Stimulation amplitude adjustion window
    _winSize = config.value("parameter/adjustwindow").toInt();
    _pBuffer = new double[_winSize];
    _idxBuffer = 0;

    // Linear modulation
    _nInit = config.value("parameter/ninit").toDouble();
    if(_nInit>0){
        _idxInit = 0;
        _pBufferInit = new double[_nInit];
        _isInit = true;
    }else{
        _isInit = false;
        _A = config.value("parameter/A").toDouble();
        _B = config.value("parameter/B").toDouble();
    }

    // Saving data to file
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


    *_stream << "clean\toscillation\tstim\tA\tB" << endl;

}

AmplModulation::~AmplModulation(){
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

void AmplModulation::recieveData(double sample){
    _pBuffer[_idxBuffer] = sample;
    _idxBuffer++;
    // Save signal
    *_stream << sample;

    if(_idxBuffer>=_winSize){
        // Calculate target oscillation amplitude
        double oscAmpl = calOscillationAmpl();
        // Save target oscillation amplitude
        *_stream << "\t" << oscAmpl;

        if(_isInit){
            // Initiating
            _pBufferInit[_idxInit] = oscAmpl;
            _idxInit++;

            if(_idxInit>=_nInit){
                // Calculate A&B
                double maxAmpl = *max_element(_pBufferInit,_pBufferInit+_nInit);
                double minAmpl = *min_element(_pBufferInit,_pBufferInit+_nInit);

                if(maxAmpl==minAmpl){
                    // Initiate again
                    _idxInit = 0;
                    _idxBuffer = 0;
                    *_stream << endl;
                    return;
                }

                _A = (_amplLimMax-_amplLimMin)/(maxAmpl-minAmpl);
                _B = _amplLimMin-_A*minAmpl;

                _isInit = false;

                // Start stimulation with initial parameters
                emit signalStimulation(_amplitude,_pulseWidth,_freq,true);

                // Save stimulation ampltude
                *_stream << "\t" << _amplitude;

                // Save A&B
                *_stream << "\t" << _A << "\t" << _B;
            }
        }else{
            // Running
            double stimAmpl = _A*oscAmpl+_B;
            double delta = stimAmpl-_amplitude;
            int sign(double x);
            if(abs(delta)>_maxRate){
                _amplitude = _amplitude+sign(delta)*_maxRate;
            }else{
                _amplitude = stimAmpl;
            }
            if(_amplitude>_amplLimMax){
                _amplitude = _amplLimMax;
            }
            if(_amplitude<_amplLimMin){
                _amplitude = _amplLimMin;
            }
            // Apply stimulation parameters
            emit signalStimulation(_amplitude,_pulseWidth,_freq,true);
            *_stream << "\t" << _amplitude;
        }
        _idxBuffer = 0;
    }
    *_stream << endl;

}

double AmplModulation::calOscillationAmpl(){
    // Get NFFT
    int nfft = _winSize;
    // Get target bin index
    int startBinIdx = floor(_freqband[0]/_fs*nfft);
    int endBinIdx = ceil(_freqband[1]/_fs*nfft);
    // Ignore DC bin
    if(startBinIdx==0)
        startBinIdx=1;
    // Allocate RAM
    kiss_fft_cpx *power = new kiss_fft_cpx[nfft/2+1];
    // Do FFT
    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft,false,0,0);
    kiss_fftr(cfg, _pBuffer, power);
    // Free RAM
    kiss_fftr_free(cfg);
    // Calculate target oscillation amplitude
    double ampl = 0;
    for(int i=startBinIdx;i<endBinIdx;i++){
        ampl = ampl+sqrt(pow(power[i].r,2)+pow(power[i].i,2));
        qDebug() << power[i].r << endl;
        qDebug() << power[i].i << endl;
    }
    ampl = ampl/nfft*2;

    return ampl;
}

int sign(double x){
    if(x>0){
        return 1;
    }else if(x<0){
        return -1;
    }else{
        return 0;
    }
}
