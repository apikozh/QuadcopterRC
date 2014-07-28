#include "grapharea.h"
#include "ui_grapharea.h"

#include <QPainter>
#include <QDebug>
#include <cmath>

GraphArea::GraphArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphArea),
    data(NULL), valsNum(0)
{
    ui->setupUi(this);

    xScale = yScale = intervalScale = 1;
    xScroll = 0;
    variableInterval = false;
    stairStep = false;

}

GraphArea::~GraphArea()
{
    delete ui;
}

void GraphArea::setData(IGraphData *data)
{
    Qt::GlobalColor defaultColors[] = {/*Qt::black,*/ Qt::blue, Qt::green, Qt::red,
                                       Qt::cyan, Qt::magenta, Qt::yellow, Qt::gray,
                                       Qt::darkBlue, Qt::darkGreen, Qt::darkRed,
                                       Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};

    this->data = data;
    valsNum = data->getSeriesNumber();
    visibilities.resize(valsNum);
    visibilities.fill(true);
    colors.resize(valsNum);
    for (int i=0; i<valsNum; i++) {
        colors[i] = QColor(defaultColors[i]);
    }

    emit scrollChanged();

    connect(data, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
    update();
}

void GraphArea::setSeriesColor(int index, QColor value)
{
    colors[index] = value;

    update();
}

QColor GraphArea::getSeriesColor(int index)
{
    return colors[index];
}

void GraphArea::setSeriesVisibility(int index, bool value)
{
    visibilities[index] = value;

    update();
}

bool GraphArea::isSeriesVisible(int index)
{
    return visibilities[index];
}

void GraphArea::setScale(float x, float y)
{
    xScroll *= x/xScale;

    xScale = x;
    yScale = y;

    emit scrollChanged();

    update();
}

void GraphArea::setIntervalScale(float value)
{
    if (intervalScale != value) {
        intervalScale = value;
        if (variableInterval)
            update();
    }
}

void GraphArea::setScroll(int value)
{
    if (xScroll != value && value >= 0 && value <= getMaxScroll()) {
        xScroll = value;
        update();
    }
}

int GraphArea::getScroll()
{
    return xScroll;
}

int GraphArea::getMaxScroll()
{
    int result = 0;

    if (data) {
        if (variableInterval)
            result = data->getFullInterval()*intervalScale*xScale - getScrollPage();
        else
            result = data->getSize()*xScale - getScrollPage();
        if (result < 0)
            result = 0;
    }

    return result;
}

int GraphArea::getScrollPage()
{
    return width();
}

void GraphArea::setStairStep(bool value)
{
    stairStep = value;
    update();
}

void GraphArea::setVariableInterval(bool value)
{
    variableInterval = value;
    emit scrollChanged();
    update();
}

void GraphArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    int size, beginIndex, endIndex;
    double beginTime = xScroll/intervalScale/xScale;
    double endTime = (xScroll + getScrollPage())/intervalScale/xScale;
    double fTime, time;
    if (data && (size = data->getSize()) > 1) {
        QPointF* polyline;
        if (variableInterval) {
            fTime = time = 0;
            for (beginIndex=0; beginIndex<size-1 && time <= beginTime; beginIndex++) {
                fTime = time;
                time += data->getInterval(beginIndex);
            }
            beginIndex--;
            time = fTime;
            for (endIndex = beginIndex; endIndex<size-1 && time < endTime; endIndex++) {
                time += data->getInterval(endIndex);
            }
            //qDebug() << beginIndex << endIndex << size;
            if (endIndex - beginIndex > 0) {
                size = endIndex - beginIndex + 1;
                polyline = new QPointF[size];
                for (int k=0; k<valsNum; k++) {
                    if (visibilities[k]) {
                        time = fTime - beginTime;
                        for (int i=0; i<size; i++) {
                            polyline[i].setX(width()-1 - time * intervalScale * xScale);
                            polyline[i].setY(height()/2.0 - data->getValue(i+beginIndex, k) * yScale);
                            time += data->getInterval(i+beginIndex);
                        }
                        painter.setPen(colors[k]);
                        painter.drawPolyline(polyline, size);
                    }
                }
                delete[] polyline;
            }
        }else{
            beginIndex = xScroll/xScale;
            endIndex = (xScroll + getScrollPage())/xScale + 1;
            if (endIndex >= size)
                endIndex = size-1;
            if (endIndex - beginIndex > 0) {
                size = endIndex - beginIndex + 1;
                polyline = new QPointF[size];
                for (int k=0; k<valsNum; k++) {
                    if (visibilities[k]) {
                        for (int i=0; i<size; i++) {
                            polyline[i].setX(width()-1 - i * xScale);
                            polyline[i].setY(height()/2.0 - data->getValue(i+beginIndex, k) * yScale);
                        }
                        painter.setPen(colors[k]);
                        painter.drawPolyline(polyline, size);
                    }
                }
                delete[] polyline;
            }
        }
        /*int p1 = ceil(ln(20/xScale/1)/ln(10));
        int p2 = ceil(ln(20/xScale/2)/ln(10));
        int p5 = ceil(ln(20/xScale/5)/ln(10));
        if (p1 < 0) p1 = 0;
        if (p2 < 0) p2 = 0;
        if (p5 < 0) p5 = 0;
        int s1 = pow(10, p1)*1;
        int s2 = pow(10, p2)*2;
        int s5 = pow(10, p5)*5;
        if (s1 < s2 && s1 < s5) {
            size = s1;
        }else if (s2 < s1 && s2 < s5) {
            size = s2;
        }else{
            size = s5;
        }
        double sPoint = -(beginIndex%size)*xScale;
        endIndex = getScrollPage()/xScale/size + 1;
        for (int)*/
        //TODO steps, stairStep
        /*
        n = ceil(ln(min/scale/a)/ln(10))
        n = ceil(ln(min/intervalScale/scale/a)/ln(10))
        */
    }

    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width()-1, height()-1);

    int x, y, p, p1, p2, p5;
    QString text;

    p1 = ceil(log(30/yScale/1)/log(10.0));
    p2 = ceil(log(30/yScale/2)/log(10.0));
    p5 = ceil(log(30/yScale/5)/log(10.0));
    p = p5;
    x = 5;
    if (p >= p2) {
        p = p2;
        x = 2;
    }
    if (p >= p1) {
        p = p1;
        x = 1;
    }
    time = pow(10.0, p)*x;

    p = -p;
    if (p < 0)
        p = 0;
    size = height()/2.0/(time*yScale) + 2;
    for (int i=1; i<size; i++) {
        y = i*time * yScale;
        painter.drawLine(width()-1 -5, height()/2-y, width()-1, height()/2-y);
        painter.drawLine(width()-1 -5, height()/2+y, width()-1, height()/2+y);
        text = QString::number(i*time, 'f', p);
        painter.drawText(
                    width()-1 -8 - painter.fontMetrics().width(text),
                    height()/2.0 - y + painter.fontMetrics().height()/2.0-2, text);
        text = QString::number(-i*time, 'f', p);
        painter.drawText(
                    width()-1 -8 - painter.fontMetrics().width(text),
                    height()/2.0 + y + painter.fontMetrics().height()/2.0-2, text);
    }

    if (variableInterval) {
        p1 = ceil(log(50/intervalScale/xScale/1)/log(10.0));
        p2 = ceil(log(50/intervalScale/xScale/2)/log(10.0));
        p5 = ceil(log(50/intervalScale/xScale/5)/log(10.0));
        p = p5;
        x = 5;
        if (p >= p2) {
            p = p2;
            x = 2;
        }
        if (p >= p1) {
            p = p1;
            x = 1;
        }
        time = pow(10.0, p)*x;

        p = -p;
        if (p < 0)
            p = 0;
        fTime = ((int)(beginTime/time))*time;
        size = (endTime - beginTime)/time + 1;
        for (int i=0; i<size; i++) {
            x = width()-1 - (fTime - beginTime + i*time) * intervalScale * xScale;
            painter.drawLine(x, height()/2-5, x, height()/2+5);
            text = QString::number(fTime + i*time, 'f', p);
            painter.drawText(
                        x - painter.fontMetrics().width(text)/2,
                        height()/2-8, text);
        }
    }else{

    }

    painter.drawLine(QPointF(0, height()/2), QPointF(width(), height()/2));


}

void GraphArea::resizeEvent(QResizeEvent* event)
{
    emit scrollChanged();
}

void GraphArea::onDataChanged()
{
    update();
    emit scrollChanged();
}
