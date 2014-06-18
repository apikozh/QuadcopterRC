#ifndef IGRAPHDATA_H
#define IGRAPHDATA_H

#include <QObject>

class IGraphData : public QObject
{
    Q_OBJECT

public:
    virtual int getSize() = 0;
    virtual int getSeriesNumber() = 0;
    virtual double getFullInterval() = 0;
    virtual float getInterval(int index) = 0;
    virtual float getValue(int index, int seriesIndex) = 0;

signals:
    void dataChanged();

};

#endif // IGRAPHDATA_H
