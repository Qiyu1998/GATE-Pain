#include "ns2dbs.h"
#include <QSettings>
#include <QDebug>

const int NUM_NODE = 2;

NS2DBS::NS2DBS(QString configFile)
{
    QSettings config(configFile, QSettings::IniFormat);

    int **node = new int*[NUM_NODE];
    for(int i=0;i<NUM_NODE;i++){
        node[i] = new int[2];
    }

    QString nodeStr1 = config.value("parameter/node1").toString();
    qDebug() << "node1: " << nodeStr1 << endl;
    QStringList spl1 = nodeStr1.split(',',QString::SkipEmptyParts);
    node[0][0] = spl1.at(0).toInt();
    qDebug() << "node1: " << node[0][0] << endl;
    node[0][1] = spl1.at(1).toInt();
    qDebug() << "node1: " << node[0][1] << endl;

    QString nodeStr2 = config.value("parameter/node2").toString();
    qDebug() << "node2: " << nodeStr2 << endl;
    QStringList spl2 = nodeStr2.split(',',QString::SkipEmptyParts);
    node[1][0] = spl2.at(0).toInt();
    qDebug() << "node2: " << node[1][0] << endl;
    node[1][1] = spl2.at(1).toInt();
    qDebug() << "node2: " << node[1][1] << endl;

    int decLevel = MAX(node[0][0],node[1][0]);
    qDebug() << "decLevel: " << decLevel << endl;

    QByteArray tempBasis = config.value("parameter/basis").toString().toLatin1();
    char *basis = tempBasis.data();
    qDebug() << "basis: " << basis << endl;

    int step = config.value("parameter/step").toInt();
    qDebug() << "step: " << step << endl;

    int win = config.value("parameter/win").toInt();
    qDebug() << "win: " << win << endl;

    int priori = config.value("parameter/priori").toInt();
    qDebug() << "priori: " << priori << endl;

    int n1 = config.value("parameter/n1").toInt();
    qDebug() << "n1: " << n1 << endl;

    int n2 = config.value("parameter/n2").toInt();
    qDebug() << "n2: " << n2 << endl;

    QByteArray tempPath = config.value("parameter/savepath").toString().toLatin1();
    char *datapath = tempPath.data();
    qDebug() << "datapath: " << datapath << endl;

    // MODE0
    _amplitude0 = config.value("mode0/amplitude").toDouble();
    qDebug() << "ampl: " << _amplitude0 << endl;
    _pulseWidth0 = config.value("mode0/pulsewidth").toDouble();
    qDebug() << "width: " << _pulseWidth0 << endl;
    _freq0 = config.value("mode0/frequency").toInt();
    qDebug() << "freq: " << _freq0 << endl;
    _stim0 = config.value("mode0/stim").toBool();
    qDebug() << "stim: " << _stim0 << endl;

    // MODE1
    _amplitude1 = config.value("mode1/amplitude").toDouble();
    qDebug() << "ampl: " << _amplitude1 << endl;
    _pulseWidth1 = config.value("mode1/pulsewidth").toDouble();
    qDebug() << "width: " << _pulseWidth1 << endl;
    _freq1 = config.value("mode1/frequency").toInt();
    qDebug() << "freq: " << _freq1 << endl;
    _stim1 = config.value("mode1/stim").toBool();
    qDebug() << "stim: " << _stim1 << endl;

    // MODE2
    _amplitude2 = config.value("mode2/amplitude").toDouble();
    qDebug() << "ampl: " << _amplitude2 << endl;
    _pulseWidth2 = config.value("mode2/pulsewidth").toDouble();
    qDebug() << "width: " << _pulseWidth2 << endl;
    _freq2 = config.value("mode2/frequency").toInt();
    qDebug() << "freq: " << _freq2 << endl;
    _stim2 = config.value("mode2/stim").toBool();
    qDebug() << "stim: " << _stim2 << endl;

    // MODE3
    _amplitude3 = config.value("mode3/amplitude").toDouble();
    qDebug() << "ampl: " << _amplitude3 << endl;
    _pulseWidth3 = config.value("mode3/pulsewidth").toDouble();
    qDebug() << "width: " << _pulseWidth3 << endl;
    _freq3 = config.value("mode3/frequency").toInt();
    qDebug() << "freq: " << _freq3 << endl;
    _stim3 = config.value("mode3/stim").toBool();
    qDebug() << "stim: " << _stim3 << endl;

    _ns = new NeuralState(node, NUM_NODE, decLevel, basis, step, win, priori, n1, n2, datapath);

    for(int i=0;i<NUM_NODE;i++){
        delete [] node[i];
    }
    delete [] node;
}

void NS2DBS::recieveData(double sample){
    int NS = 0;
    if(_ns->recieveData(sample, &NS)){
        this->stimStrategy(&NS,NUM_NODE);
    }
}

void NS2DBS::stimStrategy(int *state, int arrSize){
    Q_UNUSED(arrSize);
    /********************************************
     * Stim Strategy                            *
     * NS(0|0) --> Stimulation MODE0            *
     * NS(0|1) --> Stimulation MODE1            *
     * NS(1|0) --> Stimulation MODE2            *
     * NS(1|1) --> Stimulation MODE3            *
     ********************************************/

    if(state[0]==0){
        if(state[1]==0){
            // Stimulation MODE0
            emit signalStimulation(_amplitude0,_pulseWidth0,_freq0,_stim0);
        }else{
            // Stimulation MODE1
            emit signalStimulation(_amplitude1,_pulseWidth1,_freq1,_stim1);
        }
    }else{
        if(state[1]==0){
            // Stimulation MODE2
            emit signalStimulation(_amplitude2,_pulseWidth2,_freq2,_stim2);
        }else{
            // Stimulation MODE3
            emit signalStimulation(_amplitude3,_pulseWidth3,_freq3,_stim3);
        }
    }
}

NS2DBS::~NS2DBS(){
    if(_ns){
        delete _ns;
    }
}
