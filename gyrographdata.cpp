#include "gyrographdata.h"

GyroGraphData::GyroGraphData(TraceLogStore* data, bool filtered)
    : data(data), filtered(filtered)
{
    connect(data, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
}

int GyroGraphData::getSize()
{
    return data->getSize();
}

int GyroGraphData::getSeriesNumber()
{
    return 3;
}

QString GyroGraphData::getSeriesName(int seriesIndex)
{
    switch (seriesIndex) {
        case 0: return "Rot X (Pitch)";
        case 1: return "Rot Y (Roll)";
        case 2: return "Rot Z (Yaw)";
        default: return "";
    }
}

double GyroGraphData::getFullInterval()
{
    return data->getStoredInterval();
}

float GyroGraphData::getInterval(int index)
{
    return data->getRecord(index).fullInterval;
}

float GyroGraphData::getValue(int index, int seriesIndex)
{
    if (filtered)
        switch (seriesIndex) {
            case 0: return data->getRecord(index).fGyro[0];
            case 1: return data->getRecord(index).fGyro[1];
            case 2: return data->getRecord(index).fGyro[2];
            default: return 0;
        }
    else
        switch (seriesIndex) {
            case 0: return data->getRecord(index).gyro[0];
            case 1: return data->getRecord(index).gyro[1];
            case 2: return data->getRecord(index).gyro[2];
            default: return 0;
        }
}

void GyroGraphData::onDataChanged()
{
    emit dataChanged();
}
