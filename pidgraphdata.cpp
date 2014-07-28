#include "pidgraphdata.h"

PIDGraphData::PIDGraphData(TraceLogStore* data, PIDAxis axis)
    : data(data), axis(axis)
{
    connect(data, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
}


int PIDGraphData::getSize()
{
    return data->getSize();
}

int PIDGraphData::getSeriesNumber()
{
    return 3;
}

QString PIDGraphData::getSeriesName(int seriesIndex)
{
    switch (seriesIndex) {
        case 0: return "P";
        case 1: return "I";
        case 2: return "D";
        default: return "";
    }
}

double PIDGraphData::getFullInterval()
{
    return data->getStoredInterval();
}

float PIDGraphData::getInterval(int index)
{
    return data->getRecord(index).fullInterval;
}

float PIDGraphData::getValue(int index, int seriesIndex)
{
    switch (seriesIndex) {
        case 0: return data->getRecord(index).pid[axis].P;
        case 1: return data->getRecord(index).pid[axis].I;
        case 2: return data->getRecord(index).pid[axis].D;
        default: return 0;
    }
}

void PIDGraphData::onDataChanged()
{
    emit dataChanged();
}
