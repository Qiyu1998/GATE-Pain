#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <string>
#include <QString>
#include <QTimer>

#include "lfpchart.h"
#include "closedloop.h"


using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateMainWindow();
    int _recChannelID;


public slots:
    void updateStimState(double amplitude, double pulseWidth, int freq, bool stimOnOff);
    void updateSystemState(QString msg);
    void handleMsg(QString msg);
    void updateChart(double *pData, int N);

private slots:
    void refreshConfigFiles();
    void on_btnStim_clicked();
    void on_plainTextEdit_textChanged();
    void on_comboBoxStrategy_currentTextChanged(const QString &arg1);
    void on_btnNewConfig_clicked();
    void on_btnConnect_clicked();
    void on_btnEditConfig_clicked();
    void on_btnDeleteConfig_clicked();

private:
    Ui::MainWindow *ui;

    QLabel *_labelStatusBarConnection = nullptr;
    QLabel *_labelStatusBarStim = nullptr;

    LfpChart *_lfpChart = nullptr;

    ClosedLoop *_clobj = nullptr;

    StimChan _stimChan;
    /* ********************** *
     * System State:          *
     *      0-not connected   *
     *      1-connecting      *
     *      2-connected       *
     *      3-stimulating     *
     * ********************** */
    int _systemState;

    void iniMainWindow();

};

#endif // MAINWINDOW_H
