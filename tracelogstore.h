#ifndef DATASTORE_H
#define DATASTORE_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QTime>
#include "msg.h"

struct LogSegment {
    QString name;
    bool armed;
    QTime startTime;
    bool sequel;
};

struct LogRecord {

    float lastInterval, fullInterval;

    float rcThrottle, rcPitch, rcRoll, rcYaw;
    float pitch, roll, yaw;

    float motorFL, motorFR, motorBL, motorBR;
    PID pid[3];

    float fAccel[3], fGyro[3];
    float accel[3], gyro[3];

    LogSegment* segment;
};

class TraceLogStore : public QObject
{
    Q_OBJECT

private:
    int index, offset, maxSize;
    QVector<LogRecord> values;
    LogRecord currentRecord;
    double storedInterval;
    bool fullLoop, frozen;
    //QVector<LogSegment> segments;
    //QFile file;
    //bool saveToFile;

public:
    TraceLogStore();
    void addMessage(LogMsg4* msg);
    //void setFile(QFile file);
    void setMaxSize(int size);
    int getSize();
    void setOffset(int value);
    int getOffset();
    int getFullSize();
    double getStoredInterval();
    void clear();
    void setFrozen(bool value);

    const LogRecord& getRecord(int index = 0) const;
    const LogRecord& getCurrentRecord() const;

signals:
    void recordChanged();
    void dataChanged();
};

#endif // DATASTORE_H
