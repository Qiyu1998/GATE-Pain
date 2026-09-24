#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>
#include <QMessageBox>
#include <QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QDir>
#include <QDesktopServices>
#include <QInputDialog>
#include "strategyfactory.hpp"
#include "stimstrategy.h"

using namespace std;

QT_CHARTS_USE_NAMESPACE

const int DISP_FS = 275;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iniMainWindow();
}

void MainWindow::updateMainWindow()
{
    switch (_systemState) {
    case 0: // Not connected
        ui->btnConnect->setText(QString("Connect"));
        ui->btnConnect->setEnabled(true);
        ui->btnStim->setText(QString("Start Stimulation"));
        ui->btnStim->setEnabled(false);
        _labelStatusBarConnection->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
        _labelStatusBarStim->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
        break;
    case 1: // Connecting
        ui->btnConnect->setText(QString("Connect"));
        ui->btnConnect->setEnabled(false);
        ui->btnStim->setText(QString("Start Stimulation"));
        ui->btnStim->setEnabled(false);
        _labelStatusBarConnection->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
        _labelStatusBarStim->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
        break;
    case 2: // Connected
        ui->btnConnect->setText(QString("Disconnect"));
        ui->btnConnect->setEnabled(true);
        ui->btnStim->setText(QString("Start\nStimulation"));
        ui->btnStim->setEnabled(true);
        _labelStatusBarConnection->setPixmap(QPixmap(":/src/GreenIcon.ico").scaledToHeight(statusBar()->height()/2));
        _labelStatusBarStim->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
        break;
    case 3: // Stimulating
        ui->btnConnect->setText(QString("Disconnect"));
        ui->btnConnect->setEnabled(true);
        ui->btnStim->setText(QString("Stop\nStimulation"));
        ui->btnStim->setEnabled(true);
        _labelStatusBarConnection->setPixmap(QPixmap(":/src/GreenIcon.ico").scaledToHeight(statusBar()->height()/2));
        _labelStatusBarStim->setPixmap(QPixmap(":/src/GreenIcon.ico").scaledToHeight(statusBar()->height()/2));
        break;
    default:
        ui->plainTextEdit->appendPlainText("Unknown system state.");
    }
}

void MainWindow::iniMainWindow()
{
    //Initialize MainWindow
    ui->btnStim->setEnabled(false);

    //Initialize config
    for(int i=0;i<NUM_STIM_STRATEGY;i++) {
        //ui->comboBoxStrategy->addItem(QString::fromStdString(StimStrategyName[i]));

        ui->comboBoxStrategy->addItem(QString::fromStdString(StrategyFactory::enumToStr(StimStrategyList(i))));
    }

    //Initialize StatusBar

    QLabel *labelConnection = new QLabel("Connection",this);
    statusBar()->addPermanentWidget(labelConnection);
    _labelStatusBarConnection = new QLabel;
    _labelStatusBarConnection->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
    statusBar()->addPermanentWidget(_labelStatusBarConnection);

    QLabel *labelStim = new QLabel("Stimulation",this);
    statusBar()->addPermanentWidget(labelStim);
    _labelStatusBarStim = new QLabel;
    _labelStatusBarStim->setPixmap(QPixmap(":/src/RedIcon.ico").scaledToHeight(statusBar()->height()/2));
    statusBar()->addPermanentWidget(_labelStatusBarStim);

    //Initialize LfpChart
    _lfpChart = new LfpChart;
    _lfpChart->legend()->hide();
    _lfpChart->setAnimationOptions(QChart::NoAnimation);
    _lfpChart->setSamplingRate(DISP_FS);

    ui->qChartViewLFP->setChart(_lfpChart);
    ui->qChartViewLFP->setRenderHint(QPainter::Antialiasing);

    //Initialize plainTextEdit
    ui->plainTextEdit->setFont(QFont("Courier New",16));

    _systemState = 0; // Not connected
    updateMainWindow();
}


void MainWindow::on_btnStim_clicked()
{
    switch (_systemState) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        _clobj->startCDBS();
        break;
    case 3:
        _clobj->stopCDBS();
        break;
    }
}

void MainWindow::updateStimState(double amplitude, double pulseWidth, int freq, bool stimOnOff){
    if(stimOnOff){
        char msg[100];
        sprintf(msg,"Stim ON: %f mA, %f ms, %d Hz", amplitude, pulseWidth, freq);
        ui->plainTextEdit->appendPlainText(msg);
    }else{
        ui->plainTextEdit->appendPlainText("Stim OFF.");
    }
}

void MainWindow::updateSystemState(QString msg){
    if(msg.compare("connecting")==0){
        _systemState = 1;
        ui->plainTextEdit->appendPlainText("Connecting...");
    }else if(msg.compare("connected")==0){
        _systemState = 2;
        ui->plainTextEdit->appendPlainText("Connected.");
    }else if(msg.compare("disconnected")==0){
        _systemState = 0;
        ui->plainTextEdit->appendPlainText("Disconnected.");
    }else if(msg.compare("startstim")==0){
        _systemState = 3;
        ui->plainTextEdit->appendPlainText("Start cDBS.");
    }else if(msg.compare("stopstim")==0){
        _systemState = 2;
        ui->plainTextEdit->appendPlainText("Stop cDBS.");
    }else{
        ui->plainTextEdit->appendPlainText("Unknown message:");
        ui->plainTextEdit->appendPlainText(msg);
    }

    updateMainWindow();
}

void MainWindow::handleMsg(QString msg){
    ui->plainTextEdit->appendPlainText(msg);
    ui->plainTextEdit->appendPlainText("\n");
}

void MainWindow::updateChart(double *pData, int N){
    _lfpChart->addSamples(pData, N);
}


MainWindow::~MainWindow()
{
    if(_clobj){
        disconnect(_clobj,&ClosedLoop::updateCharts,this,&MainWindow::updateChart);
        disconnect(_clobj,&ClosedLoop::stimStateChanged,this,&MainWindow::updateStimState);
        disconnect(_clobj,&ClosedLoop::systemStateChanged,this,&MainWindow::updateSystemState);
        disconnect(_clobj,&ClosedLoop::dispMsg,this,&MainWindow::handleMsg);

        _clobj->deleteLater();
        _clobj = nullptr;
    }

    if(_labelStatusBarConnection){
        _labelStatusBarConnection->deleteLater();
        _labelStatusBarStim = nullptr;
    }

    if(_labelStatusBarStim){
        _labelStatusBarStim->deleteLater();
        _labelStatusBarStim = nullptr;
    }

    if(_lfpChart){
        _lfpChart->deleteLater();
        _lfpChart = nullptr;
    }

    delete ui;
}


void MainWindow::on_plainTextEdit_textChanged()
{
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}


void MainWindow::on_comboBoxStrategy_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    refreshConfigFiles();
}


void MainWindow::on_btnNewConfig_clicked()
{
    bool ok;
    QString fileName = QInputDialog::getText(this,"New config file","Please input the name of the new config file:",QLineEdit::Normal,"config.ini",&ok);
    if(ok&&!fileName.isEmpty()){
        QString sourceDir = QDir::currentPath()+"/Config/"+ui->comboBoxStrategy->currentText()+"/"+ui->comboBoxConfig->currentText();
        QString toDir = QDir::currentPath()+"/Config/"+ui->comboBoxStrategy->currentText()+"/"+fileName;
        if(QFileInfo(sourceDir).isFile()){
            if(QFile::copy(sourceDir, toDir))
            {
                refreshConfigFiles();
                QDesktopServices::openUrl(QUrl(toDir));
            }else{
                handleMsg("Cannot create new config file!");
            }
        }else{
            QFile file(toDir);
            file.open(QIODevice::WriteOnly);
            file.close();
            refreshConfigFiles();
            QDesktopServices::openUrl(QUrl(toDir));
        }
    }

}


void MainWindow::on_btnConnect_clicked()
{
    if(_systemState==0){
        // Load config file
        QString configFile = QDir::currentPath()+"/Config/"+ui->comboBoxStrategy->currentText()+"/"+ui->comboBoxConfig->currentText();
        if(QFileInfo(configFile).isFile()){
            QSettings config(configFile);
            /************************************
             *  Init stimulation monitor here!  *
             ************************************/
            _lfpChart->resetChart();
            StimStrategyList strategy = StimStrategyList(ui->comboBoxStrategy->currentIndex());

            _clobj = new ClosedLoop(this);

            connect(_clobj,&ClosedLoop::updateCharts,this,&MainWindow::updateChart);
            connect(_clobj,&ClosedLoop::stimStateChanged,this,&MainWindow::updateStimState);
            connect(_clobj,&ClosedLoop::systemStateChanged,this,&MainWindow::updateSystemState);
            connect(_clobj,&ClosedLoop::dispMsg,this,&MainWindow::handleMsg);

            _clobj->init(strategy, configFile);
        }else{
            handleMsg("Cannot load config file!");
        }

    }else if(_systemState==2){
        _clobj->close();
    }else if(_systemState==3){
        _clobj->close();
    }
}


void MainWindow::refreshConfigFiles()
{
    QDir qd(QDir::currentPath()+"/Config/"+ui->comboBoxStrategy->currentText());
    QFileInfoList fileList = qd.entryInfoList(QDir::Files|QDir::CaseSensitive|QDir::NoDotAndDotDot);

    ui->comboBoxConfig->clear();
    for(int i=0; i<fileList.count();i++) {
        ui->comboBoxConfig->addItem(fileList[i].fileName());
    }
}


void MainWindow::on_btnEditConfig_clicked()
{
    QString fileName = QDir::currentPath()+"/Config/"+ui->comboBoxStrategy->currentText()+"/"+ui->comboBoxConfig->currentText();
    if(QFileInfo(fileName).isFile()){
        QDesktopServices::openUrl(QUrl(fileName));
    }
}


void MainWindow::on_btnDeleteConfig_clicked()
{
    QString fileName = QDir::currentPath()+"/Config/"+ui->comboBoxStrategy->currentText()+"/"+ui->comboBoxConfig->currentText();
    if(QFileInfo(fileName).isFile()){
        QDir().remove(fileName);
        refreshConfigFiles();
    }
}

