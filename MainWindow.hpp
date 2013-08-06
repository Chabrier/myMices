#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QSharedPointer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QFile>
#include <QStatusBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
protected:
    void createGraphicView();
    void createToolBars();
    void createStatusBar();
private:
    void loadFile(QFile&);
    void resetView();
private:
    typedef QList<QPointF> posList;
    typedef QMap<QString, posList> ScenePosList;
    typedef QMapIterator<QString, posList> ScenePosListIt;
    static const int cMouseCount;

    QSharedPointer<QGraphicsScene>  mScene;
    QSharedPointer<QGraphicsView>   mGraphicalView;
    QSharedPointer<QToolBar>        mToolbar;
    QSharedPointer<QAction>         mPlayAction;
    QSharedPointer<QAction>         mOpenAction;
    QSharedPointer<QTimer>          mTimer;
    QSharedPointer<QStatusBar>      mStatusBar;

    ScenePosList mPositionList;
private slots:
    void play();
    void open();
};
#endif
