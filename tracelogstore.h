#ifndef DATASTORE_H
#define DATASTORE_H

class TraceLogStore
{
private:
    QFile file;
    bool saveToFile;
    int firstIndex, lastIndex, currentIndex, maxSize;

public:
    TraceLogStore();
    void addMessage(LogMsg* msg);
    void setFile(QFile file);

signals:

};

#endif // DATASTORE_H
