#ifndef GYROGRAPHDATA_H
#define GYROGRAPHDATA_H

#include "igraphdata.h"
#include "tracelogstore.h"

class GyroGraphData : public IGraphData
{
    Q_OBJECT

private:
    TraceLogStore* data;
    bool filtered;

public:
    explicit GyroGraphData(TraceLogStore *data, bool filtered = true);

    virtual int getSize();
    virtual int getSeriesNumber();
    virtual QString getSeriesName(int seriesIndex);
    virtual double getFullInterval();
    virtual float getInterval(int index);
    virtual float getValue(int index, int seriesIndex);

private slots:
    void onDataChanged();

};

#endif // GYROGRAPHDATA_H
