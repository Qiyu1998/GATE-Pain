#include "nsdbs.h"
#include <QSettings>
#include <QDebug>

NSDBS::NSDBS(QString configFile)
{
    QSettings config(configFile, QSettings::IniFormat);

    _amplitude = config.value("parameter/amplitude").toDouble();
    qDebug() << "ampl: " << _amplitude << endl;
    _pulseWidth = config.value("parameter/pulsewidth").toDouble();
    qDebug() << "width: " << _pulseWidth << endl;
    _freq = config.value("parameter/frequency").toInt();
    qDebug() << "freq: " << _freq << endl;

    QString nodeStr = config.value("parameter/node").toString();
    qDebug() << "node: " << nodeStr << endl;
    QStringList spl = nodeStr.split(',',QString::SkipEmptyParts);

    int **node = new int*[1];
    node[0] = new int[2];
    node[0][0] = spl.at(0).toInt();
    qDebug() << "node: " << node[0][0] << endl;
    node[0][1] = spl.at(1).toInt();
    qDebug() << "node: " << node[0][1] << endl;
    int nNode = 1;
    int decLevel = spl.at(0).toInt();
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

    _ns = new NeuralState(node, nNode, decLevel, basis, step, win, priori, n1, n2, datapath);

    for(int i=0;i<nNode;i++){
        delete [] node[i];
    }
    delete [] node;
}

void NSDBS::recieveData(double sample){
    int NS = 0;
    if(_ns->recieveData(sample, &NS)){
        this->stimStrategy(&NS,1);
    }
}

void NSDBS::stimStrategy(int *state, int arrSize){
    Q_UNUSED(arrSize);
//    /*************************************************
//     * Stim Strategy                                 *
//     * Low-beta(1)/High-beta(0|1) --> High-freq Stim *
//     * Low-beta(0)/High-beta(1)   --> Low-freq Stim  *
//     * Low-beta(0)/High-beta(0)   --> OFF Stim       *
//     *************************************************/
//    Q_UNUSED(arrSize)

//    if(state[0]==1){
//        // High-freq stimulation
//        emit signalStimulation(0.05,0.06,130,true);
//    }else if (state[1]==1) {
//        // Low-freq stimulation
//        emit signalStimulation(0.05,0.09,60,true);
//    }else {
//        // Off
//        emit signalStimulation(0,0,0,false);
//    }

    if(state[0]==0){
        emit signalStimulation(_amplitude,_pulseWidth,_freq,true);
    }else{
        emit signalStimulation(0,0,0,false);
    }
}

NSDBS::~NSDBS(){
    if(_ns){
        delete _ns;
    }
}
