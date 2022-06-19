#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QFileDialog>
#include <Canvas.h>
#include <QListWidget>
#include <QTreeWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <QComboBox>
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void clickTree();
    void drawGraph();
private:
    void animationStart();
    void animationEnd();
    int lengthOfShortestPath;
    QPushButton *fileButton, *draw, *clearButton, *changeMode, *clearResultButton;
    QLabel *label1, *label2;
    Canvas  *canvas;
    int timeId;
    QPainter *painter;
    QTreeWidget * treeWidget, *resultTree;
    QButtonGroup *searchOption;
    QRadioButton *timeSaving, *priceSaving, *timeSavingPlusWait;
    QComboBox *beginStations, *endStations;
    BusRoute *preroute;
    QVector<int> *shortestPath;
protected:
    void timerEvent(QTimerEvent *event) override;
public slots:
    void clearAll();
    void getFile();
    void compute();
    void anotherView();
    void clearResult();
};
#endif // MAINWINDOW_H
