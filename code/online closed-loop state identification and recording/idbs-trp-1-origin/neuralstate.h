#ifndef NEURALSTATE_H
#define NEURALSTATE_H

#include <QTextStream>
#include <QFile>

#include "lib/wavelib/wavelib.h"

class NeuralState:public QObject
{
    Q_OBJECT

public:
    explicit NeuralState(QObject *parent = nullptr);
    NeuralState(int **node, int nNode, int decLevel, char* basis, int step, int win, int priori, int n1, int n2, char *outpath);
    ~NeuralState();

    bool recieveData(double sample, int *NS); // Return true if there are results.

private:
    void calculate();
    void shiftPrioriData(double **wpCoef, int *state);
    void detectState(double **wpCoef, int *neuralState, int *smoothedState);
    double thSelectMinMax(double *wpCoef, int dataSize);
    double median(double *dataORG, int dataSize);
    void selMiddleNumber(double *dataORG, double *dataDST, int dataSize, int N);
    void selLastNumber(double *dataORG, double *dataDST, int dataSize, int N);
    int max(int* arr, int nArr);
    void fabsArr(double *data, int dataSize);
    double maxabs(double* pData, int arraySize);
    void createDataFile();
    void writeData2File();
    QString getTimeStamp();
    void flushData();
    void closeDataFile();

    // Output path
    QString _outpath;

    // Flags
    bool _finishIni;
    int _countBuffer;
    int _countIniTime;

    int _idxBuffer;

    // Parameters for NS detection
    int **_node = nullptr; // 384Hz (4,1) [12-24] (4,3) [24-36]
    int _nNode;
    int _decLevel;
    QString *_basis = nullptr;
    QString *_extending = nullptr;

    int _nPointStep; //Moving step
    int _nPointWin; //Moving window (with overlap)
    int _nStepPrioriWin; // For threshold estimation
    int _nStatePriori;
    int _n1;
    int _n2;

    int *_nCoefWin = nullptr;
    int *_nCoefStep = nullptr;
    int *_nCoefPrioriWin = nullptr;

    // Buffer
    double *_pBufferStep = nullptr; // Buffer for samples in one step (1-D)
    double *_pBufferWin = nullptr;  // Buffer for samples in one window (1-D)
    double *_threshold = nullptr; // Buffer for threshold (1-D)

    double **_coefs = nullptr; // Buffer for coeffients (2-D)*
    double **_coefStep = nullptr; // !!!
    double **_prioriCoefs = nullptr; // Buffer for coeffients in priori window (2-D)*
    int **_prioriStates = nullptr; // Buffer for priori states (2-D)*

    // These buffer only used for saving data
    int *_state = nullptr; // (1-D)*
    int *_smoothedState = nullptr; // (1-D)*

    // Saving results
    QFile *_pFileDwnSmp = nullptr;
    QTextStream *_pStreamDwnSmp = nullptr;
    QFile *_pFileCfs = nullptr;
    QTextStream *_pStreamCfs = nullptr;
    QFile *_pFileThr = nullptr;
    QTextStream *_pStreamThr = nullptr;
    QFile *_pFileState = nullptr;
    QTextStream *_pStreamState = nullptr;
};

#endif // NEURALSTATE_H
