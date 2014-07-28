#include "tracelogstore.h"

#include <string.h>

TraceLogStore::TraceLogStore()
{
    index = -1;
    offset = maxSize = 0;
    frozen = false;
    saveToFile = false;
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
        index++;
        if (!maxSize || values.size() < maxSize) {
            values.push_back(currentRecord);
            //index++;// = values.size()-1;
        }else{
            index %= maxSize;
            storedInterval -= values[index].fullInterval;
            values[index] = currentRecord;
        }
        storedInterval += currentRecord.fullInterval;

        emit dataChanged();

        if (saveToFile) {
            out << currentRecord.gyro[0] << "\t";
            out << currentRecord.gyro[1] << "\t";
            out << currentRecord.gyro[2] << "\t";
            out << currentRecord.accel[0] << "\t";
            out << currentRecord.accel[1] << "\t";
            out << currentRecord.accel[2] << "\t";
            out << currentRecord.fGyro[0] << "\t";
            out << currentRecord.fGyro[1] << "\t";
            out << currentRecord.fGyro[2] << "\t";
            out << currentRecord.fAccel[0] << "\t";
            out << currentRecord.fAccel[1] << "\t";
            out << currentRecord.fAccel[2] << "\n";
        }
    }


    emit recordChanged();
}

void TraceLogStore::setFile(QString fileName)
{
    saveToFile = true;
    file.setFileName(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
}

void TraceLogStore::setMaxSize(int size)
{
    if (size >= 0 && maxSize != size) {
        clear();
        maxSize = size;
        if (size > 0)
            values.reserve(size);
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
    return values.size();
}

double TraceLogStore::getStoredInterval()
{
    return storedInterval;
}

void TraceLogStore::clear()
{
    memset(&currentRecord, 0, sizeof(LogRecord));
    values.clear();
    offset = 0;
    index = -1;
    storedInterval = 0;
    emit dataChanged();
    emit recordChanged();
}

void TraceLogStore::setFrozen(bool value)
{
    if (frozen != value) {
        frozen = value;
        out << (frozen ? "frozen\n" : "unfrozen\n");
        out.flush();
    }
}

const LogRecord& TraceLogStore::getRecord(int index) const
{
    int idx = this->index - offset - index;
    if (maxSize && idx < 0)
        idx += maxSize;
    return values[idx];
}

const LogRecord &TraceLogStore::getCurrentRecord() const
{
    return currentRecord;
}

