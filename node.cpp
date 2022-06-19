#include "node.h"
Node::Node():serialNumber(-1),price(-1.0),distance(-1.0){
    this->cost = new QMap<int, double>;
}
Node::Node(int serial, int price, int distance):serialNumber(serial),price(price), distance(distance) {
    this->cost = new QMap<int, double>;
}
void Node::setPrice(double price){
    this->price = price;
}
void Node::setSerialNumber(int serial){
    this->serialNumber = serial;
}
void Node::setDistance(double distance){
    this->distance = distance;
}
double Node::getPrice() const {
    return this->price;
}
int Node::getSerialNumbver() const{
    return this->serialNumber;
}
double Node::getDistance() const{
    return this->distance;
}
Node::~Node(){
    delete this->cost;
}
