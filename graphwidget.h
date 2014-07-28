#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include "grapharea.h"

namespace Ui {
class GraphWidget;
}

class GraphWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit GraphWidget(QWidget *parent = 0);
    ~GraphWidget();
    
    GraphArea* graph();

    void setXScaleBounds(float min, float max);
    void setYScaleBounds(float min, float max);
    void setXScale(float value);
    void setYScale(float value);

private slots:
    void onScrollChanged();
    void on_xScale_valueChanged(int value);
    void on_yScale_valueChanged(int value);
    void on_xScroll_valueChanged(int value);

    void on_isVarInterval_clicked(bool checked);

private:
    Ui::GraphWidget *ui;
    float xScaleMin, xScaleMax;
    float yScaleMin, yScaleMax;

    void updateScale();

};

#endif // GRAPHWIDGET_H
