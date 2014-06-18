#ifndef PIDGRAPHDATA_H
#define PIDGRAPHDATA_H

#include "igraphdata.h"
#include "tracelogstore.h"

class PIDGraphData : public IGraphData
{
    Q_OBJECT

private:
    TraceLogStore* data;
    PIDAxis axis;

public:
    PIDGraphData(TraceLogStore* data, PIDAxis axis);

    virtual int getSize();
    virtual int getSeriesNumber();
    virtual double getFullInterval();
    virtual float getInterval(int index);
    virtual float getValue(int index, int seriesIndex);

private slots:
    void onDataChanged();

};

#endif // PIDGRAPHDATA_H
