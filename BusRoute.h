#ifndef BUSROUTE_H
#define BUSROUTE_H
#include <QVector>
#include "BusStation.h"
class BusRoute {
    private:
        int serialNumber;
        QVector<BusStation*> *stations;
        double price;
        double averTime;
        double speed;
        int length;
    public:
        BusRoute(){};
        BusRoute(int serial);
        void setPrice(double price);
        void setAverTime(double averTime);
        void setSpeed(double speed);
        double getPrice();
        int getLength();
        int getSerialNumber();
        double getAverTime();
        double getSpeed();
        BusStation* getStation(int index);
        void addStation(BusStation *sation);
};
#endif // BUSROUTE_H
