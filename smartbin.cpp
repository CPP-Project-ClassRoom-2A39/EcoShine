#include "smartBin.h"

SmartBin::SmartBin(QObject *parent)
    : QObject(parent), movementCount(0) {
}

SmartBin::~SmartBin() {
}

void SmartBin::setMovementCount(int count) {
    movementCount = count;
}

int SmartBin::getMovementCount() const {
    return movementCount;
}

QString SmartBin::getBinStatus() const {
    if (movementCount >= movementThreshold) {
        return "Videz la poubelle!";
    } else {
        return "La poubelle est vide.";
    }
}
