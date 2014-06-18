#ifndef GRAPHAREA_H
#define GRAPHAREA_H

#include <QWidget>
#include <QVector>
#include <QFile>
//#include <QResizeEvent>

#include "igraphdata.h"

namespace Ui {
class GraphArea;
}

class GraphArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit GraphArea(QWidget *parent = 0);
    ~GraphArea();

    void setData(IGraphData* data);

    void setSeriesColor(int index, QColor value);
    QColor getSeriesColor(int index);

    void setSeriesVisibility(int index, bool value);
    bool isSeriesVisible(int index);

    void setScale(float x, float y);
    void setIntervalScale(float value);
    void setScroll(int value);
    int getScroll();
    int getMaxScroll();
    int getScrollPage();
    void setStairStep(bool value);
    void setVariableInterval(bool value);

private:
    Ui::GraphArea *ui;
    IGraphData* data;

    int valsNum;
    QVector<QColor> colors;
    QVector<bool> visibilities;

    float xScale, yScale, intervalScale;
    int xScroll;
    bool variableInterval, stairStep;

    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);

private slots:
    void onDataChanged();

signals:
    void scrollChanged();

};

#endif // GRAPHAREA_H
