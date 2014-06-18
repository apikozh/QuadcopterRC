#include "tracelogstore.h"

#include <string.h>

TraceLogStore::TraceLogStore()
{
    offset = index = maxSize = 0;
    fullLoop = frozen = false;
    storedInterval = 0;
    memset(&currentRecord, 0, sizeof(LogRecord));
}

void TraceLogStore::addMessage(LogMsg4* msg)
{
    memset(&currentRecord, 0, sizeof(LogRecord));
    switch (msg->logLevel) {
        case 4:
            for(int axis=PID_PITCH; axis<=PID_YAW; axis++){
                currentRecord.accel[axis] = msg->accel[axis];
                currentRecord.gyro[axis] = msg->gyro[axis];
            }
        case 3:
            for(int axis=PID_PITCH; axis<=PID_YAW; axis++){
                currentRecord.fAccel[axis] = msg->fAccel[axis];
                currentRecord.fGyro[axis] = msg->fGyro[axis];
            }
        case 2:
            currentRecord.lastInterval = msg->lastInterval;
            currentRecord.motorFL = msg->motorFL;
            currentRecord.motorFR = msg->motorFR;
            currentRecord.motorBL = msg->motorBL;
            currentRecord.motorBR = msg->motorBR;
            for(int axis=PID_PITCH; axis<=PID_YAW; axis++){
                currentRecord.pid[axis].P = msg->P[axis];
                currentRecord.pid[axis].I = msg->I[axis];
                currentRecord.pid[axis].D = msg->D[axis];
            }
        case 1:
            currentRecord.fullInterval = msg->fullInterval;
            currentRecord.rcThrottle = msg->rcThrottle;
            currentRecord.rcPitch = msg->rcPitch;
            currentRecord.rcRoll = msg->rcRoll;
            currentRecord.rcYaw = msg->rcYaw;
            currentRecord.pitch = msg->pitch;
            currentRecord.roll = msg->roll;
            currentRecord.yaw = msg->yaw;
            break;
        default:
            return;
    }

    if (!frozen) {
        if (maxSize) {
            if (fullLoop)
                storedInterval -= values[index].fullInterval;
            values[index] = currentRecord;
            if (++index >= maxSize) {
                index = 0;
                fullLoop = true;
            }
        }else{
            values.push_back(currentRecord);
            index = values.size();
        }
        storedInterval += currentRecord.fullInterval;

        emit dataChanged();
    }

    emit recordChanged();
}

void TraceLogStore::setMaxSize(int size)
{
    if (size >= 0 && maxSize != size) {
        clear();
        maxSize = size;
        values.resize(size);
    }
}

int TraceLogStore::getSize()
{
    return getFullSize() - offset;
}

void TraceLogStore::setOffset(int value)
{
    if (value >= 0 && offset != value) {
        offset = value;
        currentRecord = getRecord();
        emit dataChanged();
        emit recordChanged();
    }
}

int TraceLogStore::getOffset()
{
    return offset;
}

int TraceLogStore::getFullSize()
{
    if (maxSize) {
        return fullLoop ? maxSize : index;
    }else
        return values.size();
}

double TraceLogStore::getStoredInterval()
{
    return storedInterval;
}

void TraceLogStore::clear()
{
    memset(&currentRecord, 0, sizeof(LogRecord));
    if (0 == maxSize) {
        values.clear();
    }
    offset = index = 0;
    fullLoop = false;
    storedInterval = 0;
    emit dataChanged();
    emit recordChanged();
}

void TraceLogStore::setFrozen(bool value)
{
    frozen = value;
}

const LogRecord& TraceLogStore::getRecord(int index) const
{
    int idx = this->index - 1 - offset - index;
    if (maxSize && idx < 0)
        idx += maxSize;
    return values[idx];
}

const LogRecord &TraceLogStore::getCurrentRecord() const
{
    return currentRecord;
}

