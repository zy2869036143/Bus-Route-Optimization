#ifndef BUSSTATION_H
#define BUSSTATION_H
#include <QString>
class BusStation{
    friend class Cancas;
    private:
        // 公交站点的名称
        QString name;
        // 公交站的物理坐标
        int x, y;
        // 公交站点被存储在数组中的索引
        int index;
        // 公交站的绘图坐标
        int drawX, drawY;
        int pass;

    public:
        BusStation();
        BusStation(QString name, int x, int y);
        void setX(int x);
        void setY(int y);
        int getX();
        int getY();
        int getIndex();
        void setIndex(int index);
        int getDrawX();
        int getDrawY();
        void setDrawX(int x);
        void setDrawY(int y);
        QString getName();
        int  getPass();
        // 唯一的public属性，表示该站点是否是乘客查询条件下的终点站。
        bool endStation;
        void setPass(int pass);
        // 判断两公交站点是否相同
        bool euqals(const BusStation * station);
        void reset();
};
#endif // BUSSTATION_H
