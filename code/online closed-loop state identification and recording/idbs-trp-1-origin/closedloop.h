#ifndef CLOSEDLOOP_H
#define CLOSEDLOOP_H

#include <QObject>
#include <QTimer>
#include <QSettings>
#include <QTextStream>
#include <QFile>
#include "stimdevice.hpp"
#include "devicefactory.hpp"
#include "stimstrategy.h"
#include "strategyfactory.hpp"
#include "resampling.h"
#include "cleandbs.h"
#include "lib/cascadeiir/cascadeiir.h"


class ClosedLoop : public QObject
{
    Q_OBJECT
public:
    explicit ClosedLoop(QObject *parent = nullptr);
    ~ClosedLoop();

    void init(StimStrategyList cDBS, QString configFile);
    void close();
    void startCDBS();
    void stopCDBS();

public slots:
    void getData();
    void doStim(double amplitude, double pulseWidth, int freq, bool stimOnOff);

signals:
    void updateCharts(double* sample, int N); // Delete sample in slot !!!
    void stimStateChanged(double amplitude, double pulseWidth, int freq, bool stimOnOff);
    void systemStateChanged(QString msg);
    void dispMsg(QString msg);

private:
    void savedata();

    StimDevice *_stimulator = nullptr;
    StimStrategy *_cDBS = nullptr;
    QTimer *_tGetData = nullptr;

    // Flags
    bool _isStim;

    // Parms
    int _recChnID;
    StimChan _stimChan;


    // Buffers
    short *_pBuffer = nullptr;
    int _bufferSize;

    double *_pDispBuffer = nullptr;
    int _idxDisp;

    short *_pSaveBuffer = nullptr;
    int _saveSize;
    int _idxSave;

    // CleanDBS
    bool _enableCleanDBS;
    CleanDBS *_cleanDBS;

    // Filter
    bool _enableFilter;
    double **_sosMat = nullptr;
    int _nSOS = 0;
    CascadeIIR *_filter = nullptr;

    bool _enableResample;
    Resampling *_rsCDBS = nullptr;
    Resampling *_rsChart = nullptr;

    // Save data
    bool _enableSaving;
    QFile *_pFileRaw = nullptr;
    QTextStream *_pStreamRaw = nullptr;

};

#endif // CLOSEDLOOP_H
