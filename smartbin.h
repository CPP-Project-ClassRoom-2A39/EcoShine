#ifndef SMARTBIN_H
#define SMARTBIN_H

#include <QObject>
#include <QString>

class SmartBin : public QObject {
    Q_OBJECT

public:
    explicit SmartBin(QObject *parent = nullptr);
    ~SmartBin();

    void setMovementCount(int count);
    int getMovementCount() const;
    QString getBinStatus() const;

private:
    int movementCount;
    const int movementThreshold = 10; // Threshold for triggering bin action
};

#endif // SMARTBIN_H
