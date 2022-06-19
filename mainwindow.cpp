#include "mainwindow.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QWidget>
#include <QPropertyAnimation>
#include <iostream>
#include "Canvas.h"
#include <QStackedLayout>
#include <QTreeWidget>
#include<string>
#include<QTextStream>
#include "BusStation.h"
#include "BusRoute.h"
#include <sstream>
#include <node.h>
#include <stack>
#include <QMessageBox>
#include <QtMath>

#include "QDebug"
using namespace std;
/*
 * BusStaion类：公交站
 * 实现
 */
BusStation::BusStation():x(0), y(0),name("NULL"), drawX(0), drawY(0),pass(0),endStation(false){}
BusStation::BusStation(QString name, int x, int y): name(name), x(x), y(y), drawX(x), drawY(y),pass(0),endStation(false){}
void BusStation::reset() {
    this->pass = 0;
    this->endStation = false;
}
void BusStation::setPass(int pass) {
    this->pass = pass;
}
int BusStation::getPass() {
    return pass;
}
int BusStation::getDrawX() {
    return drawX;
}
int BusStation::getDrawY() {
    return drawY;
}
void BusStation::setDrawX(int x) {
    drawX = x;
}
void BusStation::setDrawY(int y){
    drawY = y;
}
int BusStation::getX(){
    return this->x;
}
int BusStation::getY(){
    return  this->y;
}
void BusStation::setIndex(int index){
    this->index = index;
}
int BusStation::getIndex(){
    return this->index;
}
QString BusStation::getName(){
    return this->name;
}
bool BusStation::euqals(const BusStation *station){
    if(this->x==station->x  && this->y == station->y)
        return true;
    return false;
}
void BusStation::setX(int x) {
    this->x = x;
}
void BusStation::setY(int y){
    this->y = y;
}
/*
 * BusRoute类：公交线路
 * 类实现
 */
BusRoute::BusRoute(int serial):serialNumber(serial) {
    this->stations = new QVector<BusStation*>();
}
void BusRoute::setPrice(double price) {
    this->price = price;
}
void BusRoute::setSpeed(double speed) {
    this->speed = speed;
}
void BusRoute::setAverTime(double averTime) {
    this->averTime = averTime;
}
int BusRoute::getSerialNumber() {
    return this->serialNumber;
}
double BusRoute::getPrice(){
    return this->price;
}
double BusRoute::getSpeed(){
    return  this->speed;
}
int BusRoute::getLength(){
    return this->stations->length();
}
double BusRoute::getAverTime(){
    return this->averTime;
}
void BusRoute::addStation(BusStation *station){
    stations->append(station);
}
BusStation* BusRoute::getStation(int index){
    return stations->at(index);
}

/*
 * MainWindow类：主窗口
 */
MainWindow::MainWindow(QWidget *parent): QWidget(parent), preroute(NULL),shortestPath(NULL)
{
    /*
     * 主窗口字体和初始化大小的设置
     */
    this->setFont(QFont("宋体", 12));
    this->resize(1400,850);
    timeId = 0;

    /*
     *  QSS样式的设置
     */
    QFile file(":/button.qss");
    file.open(QFile::ReadOnly);
    // 读取qss样式文件中的所有数据
    QString stylesheet = file.readAll();
    file.close();
    //为当前界面类设置样式表
    this->setStyleSheet(stylesheet);

    /*
     * canvas:
     * 绘图组件的初始化
     */
    canvas = new  Canvas();

    /*
     * fileButton、draw:
     * 交互按钮和提示标签的初始化和设置
     */
    fileButton = new  QPushButton("导入文件", this);
    draw = new QPushButton("计算", this);
    clearButton = new QPushButton("清空", this);
    changeMode = new QPushButton("切换显示", this);
    clearResultButton = new QPushButton("清空结果", this);
    QLabel* label1 = new QLabel("起始点：", this);
    label1->setAlignment(Qt::AlignRight);
    QLabel *label2 = new QLabel("终止点：", this);
    label2->setAlignment(Qt::AlignRight);

     /*
      * treeWidget:
      * 左上角公交线路表的初始化和设置。
      */
    treeWidget= new QTreeWidget(this);
    treeWidget->setFont(QFont("宋体", 14));
    //设置列数
    treeWidget->setColumnCount(4);
    treeWidget->resize(600,300);
    treeWidget->setMinimumWidth(300);
    treeWidget->setMaximumWidth(360);
    QStringList labels;
    labels << "线路" << "票价" <<"发车间隔" << "车速";
    treeWidget->setHeaderLabels(labels);
    QFont headerFont;
    headerFont.setPointSize(12);
    treeWidget->headerItem()->setFont(0, headerFont);
    treeWidget->headerItem()->setFont(1, headerFont);
    treeWidget->headerItem()->setFont(2, headerFont);
    treeWidget->headerItem()->setFont(3, headerFont);
    treeWidget->setStyleSheet("QHeaderView::section{background:rgb(64,65,66);}");
    QBrush brush (QColor(255, 255, 255)) ;
    treeWidget->headerItem()->setForeground(0, brush);
    treeWidget->headerItem()->setForeground(1, brush);
    treeWidget->headerItem()->setForeground(2, brush);
    treeWidget->headerItem()->setForeground(3, brush);

    /*
     * searchOption、timeSaving、priceSaving、timeSavingPlusWait:
     * 单选框及单选项的初始化和设置。
     */
    searchOption = new QButtonGroup(this);
    timeSaving = new QRadioButton("①最省时路径(不考虑等车)", this);
    timeSaving->setChecked(true);
    priceSaving = new QRadioButton("②价格最低路径", this);
    timeSavingPlusWait = new QRadioButton("③最省时路径(考虑等车)",this);
    searchOption->addButton(timeSaving,0);
    searchOption->addButton(priceSaving, 1);
    searchOption->addButton(timeSavingPlusWait,2);
    /*
     * current
     * beginStations、endStations:
     * 起始站和终点站的列表选择框的初始化。
     */
    beginStations = new QComboBox(this);
    endStations = new QComboBox(this);

    /*
     * resultTree:
     * 右下方查询结果树形控件的初始化和设置。
     */
    resultTree= new QTreeWidget(this);
    resultTree->setFont(QFont("宋体", 14));
    resultTree->setColumnCount(4);
    resultTree->resize(600,300);
    resultTree->setMinimumWidth(300);
    resultTree->setMaximumWidth(360);
    QStringList resultLabels;
    resultLabels << "查询结果" << "起始站" << "终点站" << "查询条件";
    resultTree->setHeaderLabels(resultLabels);
    resultTree->headerItem()->setFont(0, headerFont);
    resultTree->headerItem()->setFont(1, headerFont);
    resultTree->headerItem()->setFont(2, headerFont);
    resultTree->headerItem()->setFont(3, headerFont);
    resultTree->setStyleSheet("QHeaderView::section{background:rgb(64,65,66);}");
    //为每一列设置宽度：
    resultTree->setColumnWidth(0, 200);
    resultTree->setColumnWidth(1, 200);
    resultTree->setColumnWidth(2, 200);
    resultTree->setColumnWidth(3, 200);
    resultTree->headerItem()->setForeground(0, brush);
    resultTree->headerItem()->setForeground(1, brush);
    resultTree->headerItem()->setForeground(2, brush);
    resultTree->headerItem()->setForeground(3, brush);


    /*
     * 界面布局的设置
     */
    QVBoxLayout * verticalSearchOptionContainer = new QVBoxLayout();
    QHBoxLayout * beginStationContainer = new QHBoxLayout();
    beginStationContainer->addWidget(label1);
    beginStationContainer->addWidget(beginStations);
    QHBoxLayout* endStationContainer = new QHBoxLayout();
    endStationContainer->addWidget(label2);
    endStationContainer->addWidget(endStations);
    verticalSearchOptionContainer->addLayout(beginStationContainer);
    verticalSearchOptionContainer->addLayout(endStationContainer);
    verticalSearchOptionContainer->addWidget(timeSaving);
    verticalSearchOptionContainer->addWidget(priceSaving);
    verticalSearchOptionContainer->addWidget(timeSavingPlusWait);
    QGroupBox  *searchGroup = new QGroupBox();
    searchGroup->setTitle("查询条件");
    searchGroup->setLayout(verticalSearchOptionContainer);

    QGroupBox  *actionGroup = new QGroupBox();
    actionGroup->setTitle("操作");
    QVBoxLayout * actionContainer = new QVBoxLayout();
    actionContainer->addWidget(fileButton);
    actionContainer->addWidget(draw);
    actionContainer->addWidget(clearButton);
    actionContainer->addWidget(changeMode);
    actionContainer->addWidget(clearResultButton);
    actionGroup->setLayout(actionContainer);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(treeWidget, 0, 0,2 ,1);
    layout->addWidget(resultTree, 2, 0,2, 1);
    layout->addWidget(canvas, 0, 1, 4 ,5);
    layout->addWidget(searchGroup, 0, 6,2,1);
    layout->addWidget(actionGroup, 2, 6,1,1);
    this->setLayout(layout);

    /*
     *  信号函数和槽函数的链接的设置
     */
    connect(fileButton, &QPushButton::clicked, this, &MainWindow::getFile);
    connect(draw, &QPushButton::clicked, this, &MainWindow::compute);
    connect(treeWidget, &QTreeWidget::clicked, this, &MainWindow::clickTree);
    connect(changeMode, &QPushButton::clicked, this, &MainWindow::anotherView);
    connect(clearResultButton, &QPushButton::clicked, this, &MainWindow::clearResult);
    connect(clearButton,  &QPushButton::clicked, this,  &MainWindow::clearAll);
    /*
     * 界面背景色的设置
     */
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background,QColor(192,192,192));
    this->setAutoFillBackground(true);
    this->setPalette(pal);



    /*
       QPropertyAnimation *animation= new QPropertyAnimation(fileButton, "geometry");
       //初始化Animation对象，指定要动画的属性

        animation->setDuration(1000);
        //设定持续时间，单位为毫秒

        animation->setStartValue(QRect(0, 0, 0, 0));
        //设定初始值

        animation->setKeyValueAt(0.4, QRect(20, 250, 20, 30));
        //设定中间关键值，第一个参数为时间百分比，第二个参数为关键值

        animation->setEndValue(QRect(250, 250, 100, 30));
        //设定结束值

        animation->setEasingCurve(QEasingCurve::OutBounce);
        //使用弹跳曲线

        animation->start();
        //启动动画
    */
}
void MainWindow::clearResult() {
    this->resultTree->clear();
}


void MainWindow::anotherView() {
    this->canvas->changeMode();
}

/*
 * getFile()函数
 * 功能:
 * 1、导入文件以及文件的解析
 * 2、生成可视化交通连接图
 * 3、线路图树形控件的添加
 * 4、起终站点列表选择框的添加
 */
void MainWindow::getFile() {
    QString fileName = QFileDialog::getOpenFileName(this,"选择txt文件",".","");
    QFile file (fileName);
    if(!file.open(QIODevice::ReadOnly))
        return ;
    QTextStream in (&file);
    in.setCodec("UTF-8");
    QString line = in.readLine();
    int stationSequence = 0;
    int scaleX = 0;
    int scaleY = 0;
    while (line.length()!=0) {
        // 下拉框站的顺序
        QStringList stringList = line.split("。");
        double price = stringList[1].split("元")[0].toDouble();
        double averTime = stringList[2].split("分钟")[0].toDouble();
        double speed = stringList[3].split("/每分钟")[0].toDouble();
        QStringList list1 = stringList[0].split("；");
        QStringList list2 = list1[0].split("：");
        int routeSerial = list2[0].toInt();
        BusRoute * route = new BusRoute(routeSerial);
        route->setPrice(price);     route->setSpeed(speed);     route->setAverTime(averTime);
        QStringList list3 = list2[1].split("(");
        QString beginStationName = list3[0];
        QStringList list4 = list3[1].split(",");
        int stationX = list4[0].toInt();
        int stationY = list4[1].split(")")[0].toInt();


        QTreeWidgetItem *topItem = new QTreeWidgetItem();
        ostringstream *outStringSt = new ostringstream();
        *outStringSt << route->getSerialNumber();
        QString s =  QString::fromStdString(outStringSt->str());

        // 设置公交线路信息的字体大小
        QFont ft;       ft.setPointSize(14);    topItem->setFont(0,ft);
        topItem->setText(0, QString (s));
        // 一个stringstream流只能用一次。
        delete outStringSt;   outStringSt = new ostringstream();
        *outStringSt << route->getPrice() << "￥";
        s = QString::fromStdString(outStringSt->str());
        topItem->setText(1, QString (s));
        // 一个stringstream流只能用一次。
        delete outStringSt;   outStringSt = new ostringstream();
        *outStringSt << route->getAverTime() << "分钟";
        s = QString::fromStdString(outStringSt->str());
        topItem->setText(2, QString (s));
        // 一个stringstream流只能用一次。
        delete outStringSt;   outStringSt = new ostringstream();
        *outStringSt << route->getSpeed() << "/分钟";
        s = QString::fromStdString(outStringSt->str());
        topItem->setText(3, QString (s));
        delete outStringSt;
        treeWidget->addTopLevelItem(topItem);
        BusStation *beginStation = new BusStation(beginStationName, stationX, stationY);
        if(scaleX < beginStation->getX())        scaleX = beginStation->getX();
        if(scaleY < beginStation->getY())        scaleY = beginStation->getY();
        ostringstream outStringSt2;
        outStringSt2 << beginStation->getName().toStdString() << "(" << beginStation->getX() << "," << beginStation->getY() << ")";
        QString s2 =  QString::fromStdString(outStringSt2.str());
        QStringList *c2 = new QStringList();
        *c2 << s2;
        QTreeWidgetItem *childItemFrist = new QTreeWidgetItem(topItem,*c2);
        // 设置某公交线路下第一个站点信息的字体大小
        QFont ft1;       ft1.setPointSize(11);    childItemFrist->setFont(0, ft1);
        childItemFrist->setTextColor(0, QColor(10, 245, 0));
        childItemFrist->setFirstColumnSpanned(true);
        int index_b = this->canvas->findStation(beginStation);
        if(index_b==-1){
            this->beginStations->addItem(s2, stationSequence);
            this->endStations->addItem(s2, stationSequence++);
            this->canvas->addStation(beginStation);
            route->addStation(beginStation);
        } else {
            route->addStation(this->canvas->getStation(index_b));
        }

        for(int i=1; i < list1.length(); ++i) {
            QStringList list3 = list1[i].split("(");
            QString stationName = list3[0];
            QStringList list4 = list3[1].split(",");
            int stationX = list4[0].toInt();
            int stationY = list4[1].split(")")[0].toInt();
            BusStation *station = new BusStation(stationName, stationX, stationY);

            ostringstream outStringSt;
            outStringSt << station->getName().toStdString() << "(" << station->getX() << "," << station->getY() << ")";
            QString s =  QString::fromStdString(outStringSt.str());

            int index_n = this->canvas->findStation(station);
            if(index_n==-1){
                this->beginStations->addItem(s,stationSequence);
                this->endStations->addItem(s, stationSequence++);
                this->canvas->addStation(station);
                route->addStation(station);
            }else{
                route->addStation(this->canvas->getStation(index_n));
            }

            if(scaleX < station->getX()) scaleX = station->getX();
            if(scaleY < station->getY()) scaleY = station->getY();
//            ostringstream outStringSt;
//            outStringSt << station->getName().toStdString() << "(" << station->getX() << "," << station->getY() << ")";
//            QString s =  QString::fromStdString(outStringSt.str());
            QStringList *c = new QStringList();
            *c << s;
            QTreeWidgetItem *childItem1 = new QTreeWidgetItem(topItem,*c);
            // 设置某公交线路下除第一个站点外的其他站点信息的字体大小
            childItem1->setFont(0, ft1);
            if (i == list1.length()-1 )
                childItem1->setTextColor(0, QColor(245, 10, 4));
            childItem1->setFirstColumnSpanned(true);
        }
//        this->canvas->setScale(scaleX, scaleY);
        this->canvas->addRoute(route);
        line = in.readLine();
    }
    this->canvas->setScale(scaleX, scaleY);
    this->canvas->drawGraph();
    file.close();
}

void MainWindow::drawGraph() {

}
void MainWindow::clearAll() {
    this->resultTree->clear();
    this->canvas->clear();
    this->beginStations->clear();
    this->endStations->clear();
    this->treeWidget->clear();
}
double getData(double time){
    double s = qCos( time * M_PI * 2 ) ;
    return s;
}

void MainWindow::animationStart() {
    if (!this->shortestPath->empty() && this->timeId==0) {
        this->timeId = this->startTimer(300);
        this->lengthOfShortestPath = this->shortestPath->length();
        this->canvas->m_series->clear();
    }
}

void MainWindow::timerEvent(QTimerEvent *event) {
    if(this->lengthOfShortestPath != 0 && event->timerId()== this->timeId && this->shortestPath != NULL ) {
            if(isVisible()) {
                this->canvas->m_series->setColor(QColor(0,0,0));
                this->canvas->m_series->clear();
                for(int i = this->shortestPath->length() - 1; i>=this->lengthOfShortestPath - 1 ; --i) {

                    BusStation * station = this->canvas->getStation(shortestPath->at(i ) - 1);
                    qDebug() <<station->getName() <<" x:" << station->getX() << " y:" << station->getY() ;
                    this->canvas->m_series->append(QPointF(station->getX(), station->getY()));
                }
                --lengthOfShortestPath;

                if (lengthOfShortestPath==0) {
                    delete this->shortestPath;
                    this->shortestPath = NULL;
                    this->lengthOfShortestPath = 0;
                    this->timeId = 0;
                }
           }
        }
}

/*
 * compute()函数:
 * 功能:
 * 1、应用Dijetestra算法、构造合适和图模型
 * 2、根据三种不同的查询条件和起、终站点信息计算得出最优路径。
 * 3、最优路径结果在右下方树形控件中的展示
 * 4、最优路径结果在canvas绘图组件中路径的高亮（绿色）显示
 */
void MainWindow::compute() {
    /*
     * 重置上次在canvas中的绘图线路
     */
    for(int i=0;i < this->canvas->stations->size(); ++i) {
        BusStation* station = this->canvas->getStation(i);
        station->reset();
    }

    /*
     * 判断起始站点是否相同
     */
    int begin = beginStations->currentData().toInt() + 1;
    int end = endStations->currentData().toInt() + 1;
    if (begin == end){
        QMessageBox box;
        box.information(this, "无效","起始站和终点站不能一样！");
        box.addButton("好的",QMessageBox::AcceptRole);
        return ;
    }

    /*
     * 获取所有公交站的数，并建立邻接矩阵。
     */
    int totalStations = this->canvas->getStationNumber();
    Node ** nodes = new Node*[totalStations+1];
    for(int j=1; j <= totalStations; ++j){
        nodes[j]  = new Node[totalStations+1];
    }

    /*
     *  根据三种不同的查询条件，以不同的方式初始化公交站邻接矩阵。
     */
     // 最便宜路径的临界矩阵初始化
    if(searchOption->checkedId()==1){
        for(int j=0; j < this->canvas->getRouteNumber(); ++j){
            BusRoute* route = this->canvas->getRoute(j);
            int row = route->getStation(0)->getIndex() + 1 ;
            double price = route->getPrice();
            for(int s=1; s < route->getLength(); ++s){
                BusStation* station = route->getStation(s);
                int col =  station->getIndex() + 1;
                double currentPrice = nodes[row][col].getPrice();
                if(currentPrice == -1 ||  currentPrice > price){
                    nodes[row][col].setPrice(price);
                    nodes[row][col].setSerialNumber(route->getSerialNumber());
                }
                row = col;
            }
        }
    }
    // 最省时间路径的邻接矩阵初始化（不考虑等车时间）
    else if (searchOption->checkedId() == 0){
        for(int j=0; j < this->canvas->getRouteNumber(); ++j){
            BusRoute* route = this->canvas->getRoute(j);
            BusStation  *station1 = route->getStation(0) ;
            int row = station1->getIndex()+1;
            double speed = route->getSpeed();
            for(int s=1; s < route->getLength(); ++s){
                BusStation* station2 = route->getStation(s);
                int col =  station2->getIndex() + 1;
                double currentTime = nodes[row][col].getPrice();
                double time = sqrt(pow(station1->getX() - station2->getX(), 2) + pow(station1->getY() - station2->getY(), 2))/speed;
                if(currentTime == -1 ||  currentTime > time){
                    nodes[row][col].setPrice(time);
                    nodes[row][col].setSerialNumber(route->getSerialNumber());
                }
                row = col;
                station1 = station2;
            }
        }
    }
    //最省时间路径的邻接矩阵初始化（考虑等车时间）
    else if(searchOption->checkedId() == 2 ) {
            /*
              *  在此查询条件下的邻接矩阵中，第i行第j列的权值乘坐某一线路公交车从i站到j站的耗时t。
              *  设乘客在i站等待某一辆公交车的时间为t1，该公交车从i站跑到j站的路程耗时为t2。
              *  那么 t = t1 + t2。显然t2直接根据i,j两站点的距离除以公交车的速度就可以得到。
              *  问题的关键是如何计算t1。
              *  默认乘客第一次查询时，整个公交系统从0时刻开始运转，即各线路公交将在发车时间间隔过去后发车
              *  根据此假设初始化最初的邻接矩阵。
              */
            for( int  j = 0 ;  j < this->canvas->getRouteNumber(); ++j ) {
                BusRoute* route = this->canvas->getRoute( j );
                BusStation  *station1 = route->getStation( 0 ) ;
                int add = route->getAverTime();
                double speed = route->getSpeed();
                int row = station1->getIndex() + 1;
                for (int s = 1; s < route->getLength(); ++ s) {
                    BusStation *station2 = route->getStation(s);
                    int col = station2->getIndex() + 1;
                    double time = sqrt(pow(station1->getX() - station2->getX(), 2) + pow(station1->getY() - station2->getY(), 2))/speed;
                    double currentTime = nodes[row][col].getPrice() ;
                    double cost = time + add;
                    nodes[row][col].cost -> insert( route->getSerialNumber() , cost );
                    if (currentTime == -1 || currentTime > cost) {
                        nodes[row][col].setPrice(cost);
                        nodes[row][col].setSerialNumber(route->getSerialNumber());
                    }
                    add = cost;
                    row = col;
                    station1 = station2;
                }
            }
    }

    /*
     * 初始化距离数组distance和前序节点数组predecessor
     */
    QVector<double> distance(totalStations+1);
    QVector<int> predecessor(totalStations+1);
    QVector<int> newReachAble;
    for(int i=1; i<= totalStations; ++i){
        double price= nodes[begin][i].getPrice();
        distance[i]  = price;
        if(price != -1){
            predecessor[i] = begin;
            newReachAble.append(i);
        }else
            predecessor[i] = -1;
    }
    distance[begin] = 0;
    predecessor[begin] = 0;

   /*
    * 开始遍历图中节点。
    */
    while(!newReachAble.empty()) {

        /*
         * 选择当前从起始点可到达的最近的一个站点，下标存储在p1中。
         */
        int i = 0;
        int p1 = newReachAble[i];
        int save = i;
        ++i;
       for(; i < newReachAble.length(); ++i){
            int p2 = newReachAble[i];
            if(distance[p1] > distance[p2]){
                p1 = p2;
                save = i;
            }
        }
        newReachAble.remove(save);

        /*
         * 更新邻接矩阵
         */
        // 最便宜路径查询条件下邻接矩阵的权值更新方法。
        if(searchOption->checkedId() == 1) {
            int serial = nodes[ predecessor[ p1 ] ][ p1 ].getSerialNumbver();
            for(int i=0; i<this->canvas->getRouteNumber(); ++i){
                BusRoute * currentRoute = this->canvas->getRoute(i);
                if(currentRoute->getSerialNumber() == serial) {
                    int row  = currentRoute->getStation(0)->getIndex() + 1;
                    for(int j=1; j < currentRoute->getLength(); ++j ){
                        int col = currentRoute->getStation(j)->getIndex() + 1;
                        if(nodes[row][col].getPrice() > 0) {
                            nodes[row][col].setPrice(0);
                            nodes[row][col].setSerialNumber(serial);
                        }
                        row = col;
                    }
                    break;
                }
            }
        }
        // 考虑等车时间的最省时路径查询条件下邻接矩阵的权值更新方法。
        else if ( searchOption->checkedId() == 2 ) {
            // 从站点predecessor[ p1 ]到达站点p1所经历的时间。
            double passedTime = nodes[ predecessor[ p1 ] ][ p1 ].getPrice();
            nodes[ predecessor[ p1 ] ][ p1 ].firstDecision = nodes[ predecessor[ p1 ] ][ p1 ].getSerialNumbver();
            for (int j=0; j < this->canvas->getRouteNumber(); ++j) {

                BusRoute *route = this->canvas->getRoute(j);
                BusStation *station1 =  route->getStation(0);
                int row = station1->getIndex() + 1;
                BusStation *station2 = route->getStation(1);
                int col = station2->getIndex() + 1;
                int serial = route->getSerialNumber();
                double speed = route->getSpeed();
                int splitTime = route->getAverTime();    // 发车间隔

                for (int p=2; p < route->getLength(); ++p) {
                    BusStation *station3 = route->getStation(p);
                    int next = station3->getIndex() + 1;
                    Node *node = &nodes[row][col];
                    /*
                      *  根据已流逝的时间passedTime，更新节点的权值。
                      *  新的权值等于原来到达此站需耗费的时间减去已经过的时间。
                      */
                    double updateWaitTime = node->cost->find(serial).value() - passedTime;
                    double drivingTime = sqrt(pow(station2->getX() - station3->getX(), 2) + pow(station2->getY() - station3->getY(), 2)) / speed;

                    /*
                      * 更新后的权值updatePrice<0表示在passedTime时间段內，之前离此站最近的公交A已经驶过了此站，需乘坐下一辆公交B。
                      * 如，之前：A------->Station；经历了passedTime时间后：B------->Station------->A
                      */
                    if ( updateWaitTime < 0) {
                        int multi = (-updateWaitTime) / splitTime ;
                        updateWaitTime += (multi + 1) * splitTime;
                        //
                        Node *nextNode = &nodes[col][next];
                        nextNode->cost->insert(serial, drivingTime + updateWaitTime);
                        auto beginItor = nextNode->cost->begin();
                        int serialOfminCost = beginItor.key();
                        double minCost = beginItor.value();


                        while (beginItor != nextNode->cost->end()) {
                             int anothorSerial = beginItor.key();
                             double anothorCost = beginItor.value();
                             if (anothorCost < minCost ) {
                                 minCost  = anothorCost;
                                 serialOfminCost = anothorSerial;
                                 nextNode->setPrice(minCost);
                                 nextNode->setSerialNumber(serialOfminCost);
                             }
                             ++beginItor;
                        }
                    } else {
                        nodes[col][next].setPrice(updateWaitTime + drivingTime);
                    }
                    station1 = station2;
                    station2 = station3;
                    row = col;
                    col = next;
                }
            }
        }
        // 不考虑等车时间的最省时路径查询条件下，邻接矩阵权值不需要更新。
        else if (searchOption->checkedId() == 0){}


        /*
         * 更新图中由起始节点可到达的点的数组predecessor及相应的距离distance数组。
         */
        for(int i=1; i <= totalStations; ++i) {
            if(nodes[p1][i].getPrice() != -1 && (predecessor[i] == -1 || distance[i]  > distance[p1] + nodes[p1][i].getPrice())){
                distance[i] = distance[p1] + nodes[p1][i].getPrice();
                if(predecessor[i] == -1 )
                    newReachAble.append(i);
                predecessor[i] = p1;
            }
        }
    }



    /*
     * 最终结果可视化
     */

    // 1. 无法到达
    if(predecessor[end]==-1) {
        ostringstream *outStringSt2 = new ostringstream();
        BusStation *beginStation = this->canvas->getStation(begin-1);
        BusStation *endStation = this->canvas->getStation(end-1);
        QStringList *c2 = new QStringList();
        *outStringSt2 << beginStation->getName().toStdString() << "(" << beginStation->getX() << "," << beginStation->getY() << ")";
        QString s2 =  QString::fromStdString(outStringSt2->str());
        *c2 << "无法到达" << s2;
        delete outStringSt2;
        outStringSt2 = new ostringstream();
        *outStringSt2 << endStation->getName().toStdString() <<"("<<endStation->getX()<<","<<endStation->getY()<<")";
         s2 = QString::fromStdString(outStringSt2->str());
         *c2 << s2;

         switch(searchOption->checkedId()) {
             case 0:
                *c2 << "①";
             break;
             case 1:
                *c2 << "②";
             break;
             case 2:
                *c2 << "③";
             break;
         }
        QTreeWidgetItem *topInfor = new QTreeWidgetItem(*c2);
        topInfor->setTextColor(0, QColor(244, 5, 5));
        topInfor->setTextColor(1, QColor(244, 5, 5));
        topInfor->setTextColor(2, QColor(244, 5, 5));
        topInfor->setTextColor(3, QColor(244, 5, 5));
        resultTree->addTopLevelItem(topInfor);
        this->canvas->update();
        return ;
    }
    // 2. 可以到达
    else {
        stack<int> stk;
        shortestPath = new QVector<int>();
        stk.push(end);
        shortestPath->append(end);
        BusStation *endStation = this->canvas->getStation(end-1);
        endStation->endStation = true;
        int lowestPrice = distance[end];
        while(predecessor[end] != 0) {
            stk.push(predecessor[end]);
            end = predecessor[end];
            shortestPath->append(end);
        }

        int preStation = stk.top();
        stk.pop();
        BusStation *beginStation = this->canvas->getStation(begin-1);
        ostringstream *outStringSt2 = new ostringstream();
        string append ="";
        if(searchOption->checkedId() == 1){
            ostringstream os;
            os  << lowestPrice  << "￥";
            append = os.str();
        }else if (searchOption->checkedId() == 0){
            ostringstream os;
            os << lowestPrice << "分钟";
            append = os.str();
        }
        else if(searchOption->checkedId() == 2){
            ostringstream os;
            os  << lowestPrice << "分钟";
            append = os.str();
        }
        *outStringSt2 << append;
        QString s2 =  QString::fromStdString(outStringSt2->str());
        QStringList *c2 = new QStringList();
        *c2 << s2;
        delete outStringSt2;
        outStringSt2 = new ostringstream();
        *outStringSt2 << beginStation->getName().toStdString() << "(" << beginStation->getX() << "," << beginStation->getY() << ")";
        s2 = QString::fromStdString(outStringSt2->str());
        *c2 << s2;
        delete outStringSt2;
        outStringSt2 = new ostringstream();
        *outStringSt2 << endStation->getName().toStdString() <<"("<<endStation->getX()<<","<<endStation->getY()<<")";
        s2 = QString::fromStdString(outStringSt2->str());
        *c2 << s2;
        delete outStringSt2;
        switch(searchOption->checkedId()){
            case 0:
               *c2 << "①";
            break;
            case 1:
               *c2 << "②";
            break;
            case 2:
               *c2 << "③";
            break;
        }

        QTreeWidgetItem *topInfor = new QTreeWidgetItem(*c2);

        QFont ft;
        ft.setPointSize(12);
        topInfor->setFont(0, ft);
        resultTree->addTopLevelItem(topInfor);
        ostringstream *subInf = new ostringstream();
        int preSerial = -1;
        int count=1;
        while(!stk.empty()) {
            int nextStation = stk.top();
            stk.pop();
            BusStation* preS = this->canvas->getStation(preStation-1);
            BusStation* nexS = this->canvas->getStation(nextStation-1);
            int serial   =  nodes[preStation][nextStation].getSerialNumbver();
            if (searchOption->checkedId()==2)
                serial = nodes[preStation][nextStation].firstDecision;
            if(preSerial == -1) {
                 *subInf << "线路:" << serial <<"     ";
                 *subInf << preS->getName().toStdString() <<"(" <<preS->getX() <<"," <<preS->getY() <<") ";
                 *subInf << nexS->getName().toStdString() <<"(" <<nexS->getX() <<"," <<nexS->getY() <<") ";
                 preS->setPass(count++);
                 nexS->setPass(count++);
                 preSerial = serial;
            }else if( preSerial == serial ){
                *subInf << nexS->getName().toStdString() <<"(" <<nexS->getX() <<"," <<nexS->getY() <<") ";
                nexS->setPass(count++);
            }else {
                preSerial = serial;
                QStringList *c = new QStringList();
                *c << QString::fromStdString(subInf->str());
                QTreeWidgetItem *sub = new QTreeWidgetItem(topInfor, *c);
                sub->setFirstColumnSpanned(true);
                QFont ft;  ft.setPointSize(11);  sub->setFont(0, ft);
                delete subInf;
                subInf = new ostringstream();
                int serial   =  nodes[preStation][nextStation].getSerialNumbver();
                *subInf << "换乘线路:" << serial<<" ";
                *subInf << preS->getName().toStdString() <<"(" <<preS->getX() <<"," <<preS->getY() <<") ";
                *subInf << nexS->getName().toStdString() <<"(" <<nexS->getX() <<"," <<nexS->getY() <<") ";
                nexS->setPass(count++);
            }

            if(stk.empty()) {
                nexS->endStation = true;
                QStringList *c = new QStringList();
                *c << QString::fromStdString(subInf->str());
                QTreeWidgetItem *sub = new QTreeWidgetItem(topInfor, *c);
                QFont ft;  ft.setPointSize(11);  sub->setFont(0, ft);
                sub->setFirstColumnSpanned(true);
                delete subInf;
            }
            preStation = nextStation;
        }
        /*
         * 更新canvas绘图组件，在其中绘制线路。
         */
        this->canvas->update();
        this->animationStart();
//        this->canvas->drawGraph();
        /*
         * 释放动态分配的邻接矩阵内存
         */
        for(int j=1; j <= totalStations; ++j){
            delete []  nodes[j];
        }
        delete[] nodes;
    }
}

void MainWindow::clickTree() {

    for(int i=0 ; i < this->canvas->getStationNumber(); ++i) {
        this->canvas->getStation(i)->reset();
    }
    int row = this->treeWidget->currentIndex().row();
    BusRoute * route = this->canvas->getRoute(row);
    for (int i = 0; i < route->getLength();++i) {
        route->getStation(i)->setPass(i+1);
    }
    preroute = route;
    this->canvas->update();

}

MainWindow::~MainWindow()
{
    delete  fileButton;
    delete draw;
    delete painter;
    delete canvas;
    delete label1;
    delete label2;
    delete searchOption;
    delete priceSaving;
    delete timeSaving;
    delete timeSavingPlusWait;
    delete beginStations;
    delete endStations;
    delete treeWidget;
}
