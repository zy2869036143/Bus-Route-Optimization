#ifndef CANVAS_H
#define CANVAS_H
#include <QWidget>
#include "BusStation.h"
#include "BusRoute.h"
#include <QtCharts>
class Canvas : public QWidget
{
    Q_OBJECT
    friend class MainWindow;
public:
    Canvas(QWidget *parent = nullptr);
    void addStation(BusStation *station);
    void setScale(int scaleX, int scaleY);
    void addRoute(BusRoute *route);
    BusRoute* getRoute(int index);
    int findStation(BusStation *station) const;
    bool existStation(BusStation *station) const;
    BusStation* getStation(int index);
    int getStationNumber() const;
    int getRouteNumber() const;
    void drawGraph();
    void changeMode();
    void clear();
    QLineSeries* m_series;
    int scaleX;
    int scaleY;
    ~Canvas();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    QChart * chart;
    QList<QSplineSeries *> * lineSeries;
    QList<QScatterSeries *> *pointSeries;

    bool change = true;
    int count;
    const int circleSize = 20;
    const int deltaCircle = circleSize/2;
    const int deltaText = 8;
    BusStation * current;
    QChartView *chartView;
    QVector<BusStation*> *stations;
    QVector<BusRoute*> *routes;
};
#endif // CANVAS_H
