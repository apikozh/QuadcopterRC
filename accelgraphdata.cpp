#include "accelgraphdata.h"

AccelGraphData::AccelGraphData(TraceLogStore* data, bool filtered)
    : data(data), filtered(filtered)
{
    connect(data, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
}

int AccelGraphData::getSize()
{
    return data->getSize();
}

int AccelGraphData::getSeriesNumber()
{
    return 3;
}

QString AccelGraphData::getSeriesName(int seriesIndex)
{
    switch (seriesIndex) {
        case 0: return "Accel X (Right)";
        case 1: return "Accel Y (Forward)";
        case 2: return "Accel Z (Up)";
        default: return "";
    }
}

double AccelGraphData::getFullInterval()
{
    return data->getStoredInterval();
}

float AccelGraphData::getInterval(int index)
{
    return data->getRecord(index).fullInterval;
}

float AccelGraphData::getValue(int index, int seriesIndex)
{
    if (filtered)
        switch (seriesIndex) {
            case 0: return data->getRecord(index).fAccel[0];
            case 1: return data->getRecord(index).fAccel[1];
            case 2: return data->getRecord(index).fAccel[2];
            default: return 0;
        }
    else
        switch (seriesIndex) {
            case 0: return data->getRecord(index).accel[0];
            case 1: return data->getRecord(index).accel[1];
            case 2: return data->getRecord(index).accel[2];
            default: return 0;
        }
}

void AccelGraphData::onDataChanged()
{
    emit dataChanged();
}
