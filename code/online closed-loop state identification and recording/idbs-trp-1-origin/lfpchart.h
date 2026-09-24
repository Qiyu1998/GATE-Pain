#ifndef LfpChart_H
#define LfpChart_H

#include <QtCharts/QChart>
#include <QtCore/QTimer>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class LfpChart : public QChart
{
public:
    LfpChart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = nullptr);
    virtual ~LfpChart();
    void setSamplingRate(int samplingRate);
    int getSamplingRate();
    void resetChart();

    void addSamples(double *pData, int arrSize);

private:
    QLineSeries *_series = nullptr;
    QValueAxis *_axis = nullptr;
    qreal _x;
    qreal _y;

    int _samplingRate;
    qreal _dx;
    qreal _xRange;
    qreal _yRange;
};

#endif // LfpChart_H
