#include "Canvas.h"
#include <sstream>
#include <QPainter>
#include <QMouseEvent>
#include <iostream>
#include <QPropertyAnimation>
using namespace  std;
/*
 * Canvas类：绘图组件
 * 类实现
 */
Canvas::Canvas(QWidget *parent): QWidget(parent),  m_series(NULL), scaleX(1), scaleY(1) {
   setAttribute(Qt::WA_StyledBackground);
    this->setStyleSheet("background-color:#FFFFFF; margin:2px;");
    this->stations = new QVector<BusStation*>();
    this->routes = new QVector<BusRoute*>();
    this->count = 0;
    this->lineSeries = new QList<QSplineSeries *>();
    this->pointSeries = new QList<QScatterSeries*>();
    /*
     * 基本步骤
     * 1、
    */
   this->chart = new QChart();
   chart->setTheme(QChart::ChartThemeHighContrast);//设置白色主题
   chart->setDropShadowEnabled(true);//背景阴影    m_chart->setAutoFillBackground(true);  //设置背景自动填充

   chart->setTitleBrush(QBrush(QColor(0,0,0)));//设置标题Brush
   chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
   chart->setTitle("物理坐标图");
   chart->createDefaultAxes();

   //修改说明样式
   chart->legend()->setVisible(true);
   chart->legend()->setAlignment(Qt::AlignBottom);//底部对齐
   chart->legend()->setBackgroundVisible(true);//设置背景是否可视
   chart->legend()->setAutoFillBackground(true);//设置背景自动填充
   chart->legend()->setColor(QColor(222,233,251));//设置颜色
   chart->legend()->setLabelColor(QColor(0,100,255));//设置标签颜色
   chart->legend()->setMaximumHeight(50);
   chart->setAnimationOptions(QChart::AllAnimations);
   chartView = new QChartView(chart);
   chartView->setRenderHint(QPainter::Antialiasing);
   QVBoxLayout *pVLayout = new QVBoxLayout(this);
   pVLayout->addWidget(chartView);
   chartView->setVisible(true);
   resize(960, 720);
}
bool Canvas::existStation(BusStation *station) const {
    for(int i=0; i < this->stations->length(); ++i){
        if (station->euqals(stations->at(i))){
            return true;
        }
    }
    return false;
}
void Canvas::changeMode() {
    change = !change;
    this->chartView->setVisible(change);
}
int Canvas::getStationNumber() const{
    return this->stations->length();
}
int Canvas::getRouteNumber() const{
    return  this->routes->length();
}
void Canvas::addRoute(BusRoute *route){
    this->routes->append(route);
}
void Canvas::clear() {
    this->stations->clear();
    this->routes->clear();
    this->m_series->clear();
    this->chart->removeAllSeries();
    this->lineSeries->clear();
    this->count = 0 ;
    this->pointSeries->clear();
    this->update();
}
BusRoute* Canvas::getRoute(int index){
    return this->routes->at(index);
}
void Canvas::setScale(int scaleX, int scaleY){
    int w = this->width() - 60 ;
    int h = this->height() - 60;
    for (int i=0; i < this->stations->length(); ++i ){
           BusStation* station = stations->at(i);
           station->setDrawX(w*station->getX()/scaleX + 30);
           station->setDrawY( h*station->getY()/scaleY + 30);
    }
}
int Canvas::findStation(BusStation *station) const{
    for(int i=0; i< this->stations->length(); ++i){
        if(station->euqals(stations->at(i))){
            return i;
        }
    }
    return -1;
}
void Canvas::drawGraph() {

    const int arrowLength = 1;
    const double arrowDegrees =0.5;

    for (int routeIndex = 0; routeIndex < this->routes->length(); ++routeIndex) {
        BusRoute *route = this->getRoute(routeIndex);
        QScatterSeries *routePointSeries = new QScatterSeries();
        routePointSeries->setPointLabelsFormat("(@xPoint,@yPoint)");
        routePointSeries->setPointLabelsVisible(true);
        ostringstream name1;
        name1 << route->getSerialNumber() <<"路站点";
        routePointSeries->setName(QString::fromStdString(name1.str()));
        QSplineSeries *routeLineSeries = new QSplineSeries();
        ostringstream name2;
        name2 << route->getSerialNumber() <<"路线路";
        routeLineSeries->setName(QString::fromStdString(name2.str()));

        BusStation *preStation = route->getStation(0);
        routePointSeries->append(preStation->getX(), preStation->getY());
        routeLineSeries->append(preStation->getX(), preStation->getY());
        this->lineSeries->append(routeLineSeries);

        this->pointSeries->append(routePointSeries);
        for (int stationIndex = 1; stationIndex < route->getLength(); ++ stationIndex) {
            BusStation *nextStation = route->getStation(stationIndex);
            routePointSeries->append(nextStation->getX(), nextStation->getY());
            routeLineSeries->append(nextStation->getX(), nextStation->getY());
            preStation = nextStation;
        }
        this->chart->addSeries(routePointSeries);
        this->chart->addSeries(routeLineSeries);
    }
    m_series = new QLineSeries();
    m_series->setColor(QColor(0,0,0));
    this->chart->addSeries(m_series);
    chart->createDefaultAxes();             //或者创建默认轴


}
void Canvas::paintEvent(QPaintEvent *event) {

    const int arrowLength = 20;
    const double arrowDegrees =0.5;
    QPainter painter(this);
    //设置渲染提示为消除锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setWidthF(2);
    QPen beginPen;
    beginPen.setColor(QColor(137, 107, 76));
    beginPen.setWidthF(2);
    beginPen.setBrush(QColor(21,255,12));
    QPen orange;
    orange.setColor(QColor(237, 117, 56));
    orange.setWidthF(2);
    painter.setPen(pen);
    ostringstream outStringSt;
    outStringSt <<this->stations->length() << "站" << this->routes->length() << "线";
    QString s =  QString::fromStdString(outStringSt.str());
    painter.drawText(width()-100, 30,s);


    //——————
    //    static QList<QPointF> points = QList<QPointF>() << QPointF(0, 0) << QPointF(100, 100) << QPointF(200, -100)
    //                                                        << QPointF(300, 100) << QPointF(330, -80) << QPointF(350, -70);
    //        QPainterPath path(points[0]);

    //        for (int i = 0; i < points.size() - 1; ++i) {
    //            // 控制点的 x 坐标为 sp 与 ep 的 x 坐标和的一半
    //            // 第一个控制点 c1 的 y 坐标为起始点 sp 的 y 坐标
    //            // 第二个控制点 c2 的 y 坐标为结束点 ep 的 y 坐标
    //            QPointF sp = points[i];
    //            QPointF ep = points[i+1];
    //            QPointF c1 = QPointF((sp.x() + ep.x()) / 2, sp.y());
    //            QPointF c2 = QPointF((sp.x() + ep.x()) / 2, ep.y());

    //            path.cubicTo(c1, c2, ep);
    //        }
    //        painter.setRenderHint(QPainter::Antialiasing, true);
    //        painter.setPen(QPen(Qt::black, 2));

    //        // 绘制 path
    //        painter.translate(40, 130);
    //        painter.drawPath(path);

    //        // 绘制曲线上的点
    //        painter.setBrush(Qt::gray);
    //        for (int i = 0; i < points.size(); ++i) {
    //            painter.drawEllipse(points[i], 4, 4);
    //        }
    //————————

    painter.drawText(width()/2, 30, "抽象连接图");


    for(int i=0; i < this->routes->length(); ++i) {

       BusRoute *route = this->routes->at(i);
       BusStation* preStation = route->getStation(0);

       painter.setBrush(QBrush(QColor(1, 101, 187)));

        for(int j=1; j < route->getLength() ; ++j){
            BusStation* nextStation = route->getStation(j);

            double angle = atan2((nextStation->getDrawY() - preStation->getDrawY()) ,(nextStation->getDrawX() - preStation->getDrawX()));
            double midX = (preStation->getDrawX() + nextStation->getDrawX())/2;
            double midY = (preStation->getDrawY() + nextStation->getDrawY())/2;
            double x1 = midX -  arrowLength * cos(angle - arrowDegrees);
            double y1 = midY -  arrowLength * sin(angle - arrowDegrees);
            double x2 = midX - arrowLength * cos(angle + arrowDegrees);
            double y2 = midY - arrowLength * sin(angle + arrowDegrees);

            if( preStation->getPass()>0 && nextStation->getPass() >0 && abs(nextStation->getPass() - preStation->getPass()) == 1  ){
                painter.setPen(beginPen);
//                painter.setBrush(QBrush)
            }
            else {
                painter.setPen(pen);
                painter.setBrush(QBrush(QColor(1, 101, 187)));
            }

            // 画两站点之间的连线
            //            QPointF prePoint(preStation->getDrawX(), preStation->getDrawY());
            //            QPointF nextPoint(nextStation->getDrawX(), nextStation->getDrawY());
            //            QPainterPath path(prePoint);
            //            QPointF temp1((prePoint.x() + nextPoint.x())/2, prePoint.y());
            //            QPointF temp2 ((prePoint.x() + nextPoint.x())/2, nextPoint.y());
            //            path.cubicTo(temp1, temp2, nextPoint);

            //            painter.drawPath(path);

            painter.drawLine(preStation->getDrawX(), preStation->getDrawY(), nextStation->getDrawX(), nextStation->getDrawY());
            // 画下一站的名称
            painter.drawText(nextStation->getDrawX() + deltaText , nextStation->getDrawY() - deltaText, nextStation->getName());
            if( preStation->getPass()>0 && nextStation->getPass() >0 && nextStation->getPass() - preStation->getPass() == 1  ) {
                painter.setPen(orange);
                painter.setBrush(QBrush(QColor(0,255,0)));
            }
            else {
                painter.setPen(pen);
                painter.setBrush(QBrush(QColor(1, 101, 187)));
            }

            // 画从前一站指向后一站的箭头
            painter.drawLine(x1, y1, midX, midY);
            painter.drawLine(x2, y2, midX, midY);

            if(preStation->endStation) {
                painter.setBrush(QBrush(QColor(255, 21, 27)));
                painter.setPen(pen);
            }
            else if(preStation->getPass()<=0){
                painter.setPen(orange);
                painter.setBrush(QBrush(QColor(1, 101, 187)));
            }
            else if(preStation->getPass()==1) {
                painter.setBrush(QBrush(QColor(1, 251, 187)));
                painter.setPen(beginPen);
            }else if(preStation->getPass() > 1) {
                painter.setBrush(QBrush(QColor(237, 117, 56)));
                painter.setPen(orange);
            }
            // 画前一站的圆圈和名称
            painter.drawEllipse(preStation->getDrawX()   - deltaCircle , preStation->getDrawY()  -  deltaCircle , circleSize , circleSize);
            painter.drawText(preStation->getDrawX() + deltaText ,preStation->getDrawY() - deltaText, preStation->getName());
            if(nextStation->endStation) {
                painter.setBrush(QBrush(QColor(255, 21, 27)));
            }
            else  if(nextStation->getPass()>0){
                painter.setBrush(QBrush(QColor(237, 117, 56)));
                painter.setPen(beginPen);
            }
            else {
                painter.setBrush(QBrush(QColor(1, 101, 187)));
            }
            // 画后一站的圆圈
            painter.drawEllipse(nextStation->getDrawX() - deltaCircle , nextStation->getDrawY() - deltaCircle, circleSize , circleSize);
            preStation = nextStation;
       }
    }
    painter.end();
}

void Canvas::addStation(BusStation *station){
//    if(!this->existStation(station)){
        station->setIndex(count++);
        this->stations->append(station);
   // }
}
BusStation* Canvas::getStation(int index){
    return this->stations->at(index);
}

void Canvas::mouseMoveEvent(QMouseEvent *event){
    if(current == NULL) return;
    QPoint pos = event->pos();
    if(pos.x()<width()-50 && pos.x() > 50 )
        current->setDrawX(pos.x());
    if(pos.y()<height()-50 && pos.y() >50)
        current->setDrawY(pos.y());
    update();
}
void Canvas::mousePressEvent(QMouseEvent *event){
    QPoint pos = event->pos();
    for(int i=0; i < this->stations->length(); ++i){
        int dx = stations->at(i)->getDrawX() - pos.x();
        int dy = stations->at(i)->getDrawY() - pos.y();
        int dist = sqrt(dx*dx + dy*dy);
        if (dist <  circleSize){
            current = stations->at(i);
            return;
        }
    }
}
void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    current = NULL;
}

Canvas::~Canvas(){
    delete[] routes;
    delete[] stations;
}
