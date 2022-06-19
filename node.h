#ifndef NODE_H
#define NODE_H
#include<QMap>

class Node
{
public:
    Node();
    Node(int serial, int price, int distance);
    void setPrice(double price);
    double getPrice() const;
    void setSerialNumber(int serial);
    int getSerialNumbver() const;
    void setDistance(double distance);
    double getDistance() const;
    ~Node();
    QMap<int, double> *cost;
    int firstDecision = -1;
private:
    int serialNumber;
    double price;
    double distance;
};

#endif // NODE_H
