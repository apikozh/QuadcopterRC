#ifndef GRAPHAREA_H
#define GRAPHAREA_H

#include <QWidget>
#include <QVector>
#include <QFile>

namespace Ui {
class GraphArea;
}

class GraphArea : public QWidget
{
    Q_OBJECT
    
public:
    explicit GraphArea(QWidget *parent = 0);
    ~GraphArea();

    void setScale(double x, double y);
    void setOffset(int x);
    void setStairStep(bool value);
    void clear();
    
private:
    Ui::GraphArea *ui;
    QVector<QVector<double> > values;
    //QVector<double> intervals;
    double xScale, yScale;
    int lastIndex, maxSize, xScroll;
    bool variableInterval, stairStep;

};

#endif // GRAPHAREA_H
