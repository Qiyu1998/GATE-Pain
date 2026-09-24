#include "neuralstate.h"
#include <QDebug>
#include <QDateTime>
#include <math.h>
#include <QDir>

NeuralState::NeuralState(QObject *parent) : QObject(parent)
{

}

NeuralState::NeuralState(int **node, int nNode, int decLevel, char* basis, int step, int win, int priori, int n1, int n2, char* outpath){

    // Copy parameters
    _node = new int*[nNode];
    for(int i=0;i<nNode;i++){
        _node[i] = new int[2];
        _node[i][0] = node[i][0];
        _node[i][1] = node[i][1];
    }

    _nNode = nNode;
    _decLevel = decLevel;

    _extending = new QString("sym");

    _basis = new QString(basis);
    _nPointStep = step;
    _nPointWin = win;
    _nStepPrioriWin = priori;
    _n1 = n1;
    _n2 = n2;

    // Init parameters
    _finishIni = false;
    _countBuffer = 0;
    _countIniTime = 0;
    _idxBuffer = 0;

    _nCoefWin = new int[_nNode];
    _nCoefStep = new int[_nNode];
    _nCoefPrioriWin = new int[_nNode];
    for(int i=0; i<_nNode; i++){
        int n = _node[i][0];
        _nCoefWin[i] = ceil(_nPointWin/pow(2,n));
        _nCoefStep[i] = ceil(_nPointStep/pow(2,n)); // !!!  每个step有多少系数 之前有+1 gamma不用
        _nCoefPrioriWin[i] = _nStepPrioriWin*_nCoefStep[i]; // !!!
    }

    _nStatePriori = qMax(_n1, _n2);

    // Init buffers
    int nBuffer;

    _coefs = new double* [_nNode];
    nBuffer = this->max(_nCoefWin, _nNode);
    for(int i=0;i<_nNode;i++){
        _coefs[i] = new double[nBuffer]{0};
    }

    // !!!
    _coefStep = new double* [_nNode];
    nBuffer = this->max(_nCoefStep, _nNode);
    for(int i=0;i<_nNode;i++){
        _coefStep[i] = new double[nBuffer]{0};
    }

    _prioriCoefs = new double* [_nNode];
    nBuffer = this->max(_nCoefPrioriWin, _nNode);
    for(int i=0;i<_nNode;i++){
        _prioriCoefs[i] = new double[nBuffer]{0};
    }

    _prioriStates = new int* [_nNode];
    for(int i=0;i<_nNode;i++){
        _prioriStates[i] = new int[_nStatePriori+1]{0};
    }

    _pBufferStep = new double[_nPointStep]{0};
    _pBufferWin = new double[_nPointWin]{0};
    _threshold = new double[_nNode]{0};

    _state = new int[_nNode]{0};
    _smoothedState = new int[_nNode]{0};

    // Init wavelet
    wave_object wObj = wave_init(_basis->toLatin1().data());
    wtree_object wtObj = wtree_init(wObj, _nPointWin, _decLevel);
    setWTREEExtension(wtObj, _extending->toLatin1().data());
    wtree(wtObj, _pBufferWin);
    wave_free(wObj);
    wtree_free(wtObj);

    // Create data file
    _outpath.append(outpath);
    this->createDataFile();
}

bool NeuralState::recieveData(double sample, int *NS){
    _pBufferStep[_countBuffer] = sample;
    _countBuffer++;

    // Data ready for calculation
    if(_countBuffer >= _nPointStep){
        int startIdx = _nPointWin-_nPointStep;
        for (int j(0); j<_nPointStep; j++) {
            _pBufferWin[startIdx+j] = _pBufferStep[j];
        }

        this->calculate();
        _countBuffer = 0;
        // Return NS
        for (int i=0; i<_nNode; i++) {
            NS[i] = _smoothedState[i];
        }
        return true;
    }

    return false;
}

void NeuralState::calculate(){
    wave_object wObj = wave_init(_basis->toLatin1().data());
    wtree_object wtObj = wtree_init(wObj, _nPointWin, _decLevel);
    setWTREEExtension(wtObj, _extending->toLatin1().data());
    wtree(wtObj, _pBufferWin);

    for (int i=0; i<_nNode; i++){
        int nCoefWinPadding = getWTREENodelength(wtObj, _node[i][0]);
        double *temp = new double[nCoefWinPadding];
        getWTREECoeffs(wtObj,_node[i][0],_node[i][1],temp, nCoefWinPadding);
        selMiddleNumber(temp, _coefs[i], nCoefWinPadding, _nCoefWin[i]);
        selLastNumber(_coefs[i],_coefStep[i],_nCoefWin[i],_nCoefStep[i]);
        delete[] temp;
    }

    wave_free(wObj);
    wtree_free(wtObj);

    if(_finishIni){
        double **coefs = new double* [_nNode];
        for(int i=0;i<_nNode;i++){
            coefs[i] = new double[_nCoefStep[i]]{0}; // !!!
        }

        this->detectState(coefs, _state, _smoothedState);

        this->writeData2File();// Make sure all related variables are updated before this point
        this->shiftPrioriData(_coefStep,_state);

        for(int i=0;i<_nNode;i++){
            delete [] coefs[i];
        }
        delete [] coefs;
    }else {
        int *state = new int[_nNode]{0};
        this->writeData2File();
        this->shiftPrioriData(_coefStep, state); // !!!
        _countIniTime++;
        if(_countIniTime>=_nStepPrioriWin){
            _finishIni = true;
        }

        delete [] state;
    }
}

void NeuralState::shiftPrioriData(double **wpCoef, int *state){
    for(int i=0; i<_nNode; i++){
        //Shift _prioriCoefs
        //Stop update coefs if state==1
        if(_smoothedState[i]==0){
            for(int j(0); j<_nCoefPrioriWin[i]-_nCoefStep[i]; j++){ // !!!
                _prioriCoefs[i][j] = _prioriCoefs[i][j+_nCoefStep[i]];
            }
            for(int j(0); j<_nCoefStep[i]; j++){
                _prioriCoefs[i][_nCoefPrioriWin[i]-j-1] = fabs(wpCoef[i][_nCoefStep[i]-j-1]);
                }
        }

        //Shift _prioriStates
        for(int j(0); j<_nStatePriori-1; j++){
            _prioriStates[i][j] = _prioriStates[i][j+1];
        }
        _prioriStates[i][_nStatePriori-1] = state[i];
    }



    //Shift _pBuffer
    for(int j(0); j<_nPointWin-_nPointStep; j++){
        _pBufferWin[j] = _pBufferWin[j+_nPointStep];
    }
}

void NeuralState::detectState(double **wpCoef, int *neuralState, int *smoothedState){
    for(int i=0; i<_nNode; i++){
        _threshold[i] = thSelectMinMax(_prioriCoefs[i], _nCoefPrioriWin[i]);

        int count = 0;
        for(int j(0); j<_nCoefStep[i]; j++){ // !!!
            if(fabs(_coefStep[i][j])>_threshold[i]){
                count++;
            }
        }

        int state = 0;
        if(count>=ceil(_nCoefStep[i]*0.5)){ // !!! 多少点大于阈值 当前窗判定为1
            state = 1;
        }

        // Smooth states
        int temp = state;
        int lastState = _smoothedState[i];
        if(temp!=lastState){
            if(lastState==0){                   //
                for(int j(0); j<_n1; j++){
                    if(_prioriStates[i][_nStatePriori-j-1]!=1){
                        temp = 0;
                        break;
                    }
                }
            }else {
                for(int j(0); j<_n2; j++){
                    if(_prioriStates[i][_nStatePriori-j-1]!=0){
                        temp = 1;
                        break;
                    }
                }
            }
        }

        neuralState[i] = state;
        smoothedState[i] = temp;

        for(int j(0); j<_nCoefStep[i]; j++){ // !!!
            wpCoef[i][j] = _coefStep[i][j];
        }
    }
}

double NeuralState::thSelectMinMax(double *wpCoef, int dataSize){
    double delta = median(wpCoef, dataSize)/0.6745;
    double threshold = delta*(0.3936+0.1829*(log(dataSize)/log(2)));
    return threshold;
}

double NeuralState::median(double *dataORG, int dataSize){
    double *data = new double[dataSize];
    memcpy(data, dataORG, sizeof(double)*dataSize);

    double temp;
    double result;

    for(int i=0;i<dataSize-1;i++){
        for(int j=i+1;j<dataSize;j++){
            if (data[i]>data[j])
            {
                temp=data[i];
                data[i]=data[j];
                data[j]=temp;
            }
        }
    }
    if (dataSize%2!=0){
        result = data[dataSize/2];
    }else{
        result = (data[dataSize/2]+data[dataSize/2-1])/2;
    }

    delete[] data;
    return result;
}

void NeuralState::selMiddleNumber(double *dataORG, double *dataDST, int dataSize, int N){
    int startPoint = (dataSize-N-(N%2^dataSize%2))/2;

    for(int i(0); i<N; i++){
        dataDST[i] = dataORG[i+startPoint];
    }
}

void NeuralState::selLastNumber(double *dataORG, double *dataDST, int dataSize, int N){
    int startPoint = dataSize-N;

    for(int i(0); i<N; i++){
        dataDST[i] = dataORG[i+startPoint];
    }
}

int NeuralState::max(int* arr, int nArr){
    int s = arr[0];
    for(int i=1; i<nArr; i++){
        if(arr[i]>s)
            s = arr[i];
    }
    return s;
}

void NeuralState::fabsArr(double *data, int dataSize){
    for(int i(0); i<dataSize; i++){
        data[i] = fabs(data[i]);
    }
}

double NeuralState::maxabs(double *pData, int arraySize){
    double max = pData[0];
    for (int i(1); i<arraySize; i++) {
        double temp = fabs(pData[i]);
        if (max<temp){
            max = temp;
        }
    }
    return max;
}

void NeuralState::createDataFile(){
    QDateTime datetime;
    QString timestr = datetime.currentDateTime().toString("yyyyMMddHHmmss");
    QString fileName;

    // Create folder
    QDir dir;
    if (!dir.exists(_outpath))
    {
        dir.mkpath(_outpath);
    }

    // Time stamp
    fileName = _outpath + "latestfile.txt";
    QFile *file = new QFile(fileName);
    if(!file->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << fileName;
    QTextStream *stream = new QTextStream(file);
    *stream << timestr;
    file->flush();
    file->close();
    file->deleteLater();
    delete stream;

    // Down-sampled data
    fileName = _outpath + "DWNSMP-" + timestr + ".txt";
    _pFileDwnSmp = new QFile(fileName);
    if(!_pFileDwnSmp->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << fileName;

    _pStreamDwnSmp = new QTextStream(_pFileDwnSmp);

    // Coefficients
    fileName = _outpath + "CFS-" + timestr + ".txt";
    _pFileCfs = new QFile(fileName);
    if(!_pFileCfs->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << fileName;

    _pStreamCfs = new QTextStream(_pFileCfs);

    // Threshold
    fileName = _outpath + "THR-" + timestr + ".txt";
    _pFileThr = new QFile(fileName);
    if(!_pFileThr->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << fileName;

    _pStreamThr = new QTextStream(_pFileThr);

    // State
    fileName = _outpath + "STATE-" + timestr + ".txt";
    _pFileState = new QFile(fileName);
    if(!_pFileState->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << fileName;

    _pStreamState = new QTextStream(_pFileState);

    // Info
    fileName = _outpath + "INFO-" + timestr + ".txt";
    QFile *info = new QFile(fileName);
    if(!info->open(QIODevice::ReadWrite|QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return;
    }
    qDebug() << "Saving data in:" << fileName;

    QTextStream pStremInfo(info);

    pStremInfo << "Item\tValue" << endl;
    pStremInfo << "node\t";
    for (int i=0; i<_nNode; i++) {
        pStremInfo << _node[i][0] << "," << _node[i][1] <<"  ";
    }
    pStremInfo << endl;

    pStremInfo << "basis\t" << _basis->toLatin1().data() << endl;
    pStremInfo << "step\t" << _nPointStep <<endl;
    pStremInfo << "win\t" << _nPointWin <<endl;
    pStremInfo << "pri\t" <<_nStepPrioriWin*_nPointStep << endl;
    pStremInfo << "n1\t" << _n1 <<endl;
    pStremInfo << "n2\t" << _n2 << endl;

    return;
}

void NeuralState::writeData2File(){
    QString timeStamp = this->getTimeStamp();

    // Down-sampled data
    *_pStreamDwnSmp << timeStamp << "\t";
    for (int i(0); i<_nPointStep; i++) {
        *_pStreamDwnSmp << _pBufferStep[i] << "\t";
    }
    *_pStreamDwnSmp << endl;

    // Coefficients
    for(int i=0; i<_nNode; i++){
        *_pStreamCfs << timeStamp << "\t" << "NS" << i << "\t";
        for (int j(0); j<_nCoefStep[i]; j++) {
            *_pStreamCfs << _coefStep[i][j] << "\t";
        }
        *_pStreamCfs << endl;
    }

    // Threshold
    *_pStreamThr << timeStamp << "\t";
    for(int i=0; i<_nNode; i++){
        *_pStreamThr << "Thr" << i << "\t";
        *_pStreamThr << _threshold[i] <<"\t";
    }
    *_pStreamThr << endl;

    // States
    *_pStreamState << timeStamp << "\t";
    for(int i=0; i<_nNode; i++){
        // State
        *_pStreamState << "ORG" << i << "\t";
        *_pStreamState << _state[i] <<"\t";
        // Smoothed State
        *_pStreamState << "SMT" << i << "\t";
        *_pStreamState << _smoothedState[i] <<"\t";
    }
    *_pStreamState << endl;
}

QString NeuralState::getTimeStamp(){
    QDateTime datetime;
    QString timestr=datetime.currentDateTime().toString("hh:mm:ss.zzz");
    return timestr;
}

void NeuralState::flushData(){
    _pFileDwnSmp->flush();
    _pFileCfs->flush();
    _pFileThr->flush();
    _pFileState->flush();
}

void NeuralState::closeDataFile(){
    this->flushData();

    if(_pFileDwnSmp){
        _pFileDwnSmp->close();
        _pFileDwnSmp->deleteLater();
    }

    if(_pFileCfs){
        _pFileCfs->close();
        _pFileCfs->deleteLater();
    }

    if(_pFileThr){
        _pFileThr->close();
        _pFileThr->deleteLater();
    }

    if(_pFileState){
        _pFileState->close();
        _pFileState->deleteLater();
    }

    if(_pStreamDwnSmp){
        delete _pStreamDwnSmp;
    }

    if(_pStreamCfs){
        delete _pStreamCfs;
    }

    if(_pStreamThr){
        delete _pStreamThr;
    }

    if(_pStreamState){
        delete _pStreamState;
    }
}

NeuralState::~NeuralState(){
    // Close data file and free handle
    this->closeDataFile();

    // Free
    delete _basis;
    delete _extending;

    delete [] _nCoefWin;
    delete [] _nCoefStep;
    delete [] _nCoefPrioriWin;

    // Free buffers
    delete [] _pBufferStep;
    delete [] _pBufferWin;
    delete [] _threshold;

    for(int i(0); i<_nNode; i++){
        delete [] _coefs[i];
        delete [] _coefStep[i];
        delete [] _prioriCoefs[i];
        delete [] _prioriStates[i];
        delete [] _node[i];
    }

    delete [] _state;
    delete [] _smoothedState;
}
