#include "lfpchart.h"

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QTime>
#include <QtCore/QDebug>

LfpChart::LfpChart(QGraphicsItem *parent, Qt::WindowFlags wFlags):
    QChart(QChart::ChartTypeCartesian, parent, wFlags),
    _series(nullptr),
    _axis(new QValueAxis),
    _x(0),
    _y(0),
    _xRange(1),
    _yRange(200)
{
    _series = new QLineSeries(this);
    QPen blue(Qt::blue);
    blue.setWidth(1);
    _series->setPen(blue);

    // Enable OpenGL accelerator
    _series->setUseOpenGL(true);

    addSeries(_series);
    createDefaultAxes();

    setAxisX(_axis, _series);
    _axis->setMinorTickCount(5);
    _axis->setTickCount(static_cast<int>(_xRange+1));
    _axis->setLabelFormat("%d");
    axisX()->setLabelsFont(QFont("Courier New",16));
    axisY()->setLabelsFont(QFont("Courier New",12));
    axisX()->setRange(0,_xRange);
    axisY()->setRange(-_yRange,_yRange);

}

void LfpChart::addSamples(double *pData, int arrSize)
{
    int mx(0);
    qreal d = plotArea().width() / (_xRange*_samplingRate);
    for (int i(0); i<arrSize; i++) {
        _y = pData[i];
        _series->append(_x, _y);
        _x += _dx;
        if (_x >= _xRange) {
            _series->remove(0);
            mx++;
        }
    }

    // Auto reset the range of y axis
    QVector<QPointF> points = _series->pointsVector();

    qreal max = 0;
    qreal yPoint = 0;
    QVector<QPointF>::iterator iter;
    for (iter=points.begin();iter!=points.end();iter++)
    {
        yPoint = qAbs(iter->ry());
        if (max<yPoint) {
            max = yPoint;
        }
    }

    if (max>_yRange) {
        _yRange = max*1.1;
    } else if (max<0.7*_yRange) {
        _yRange = max*1.1;
    }

    if(_yRange<150){
        _yRange = 150;
    }

    axisY()->setRange(-_yRange,_yRange);

    scroll(d*mx, 0);

    delete[] pData;
    pData = nullptr;
}

void LfpChart::setSamplingRate(int samplingRate)
{
    _samplingRate = samplingRate;
    _dx = qreal(1)/samplingRate;
}

int LfpChart::getSamplingRate()
{
    return _samplingRate;
}

void LfpChart::resetChart()
{
    _x = 0;
    _series->clear();
    axisX()->setRange(0,_xRange);
}

LfpChart::~LfpChart()
{
    if(_series){
        _series->deleteLater();
        _series = nullptr;
    }
}
