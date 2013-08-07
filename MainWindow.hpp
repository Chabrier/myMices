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
#include <QSpinBox>

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
    typedef enum {IDLE,PLAY,PAUSE} sceneState;

    void loadFile(QFile&);
    void resetView();
    void setSceneState(sceneState);
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
    QSharedPointer<QSpinBox>  mSpeedController;
    QSharedPointer<QSpinBox>  mZoomController;
    QSharedPointer<QTimer>          mTimer;
    QSharedPointer<QStatusBar>      mStatusBar;

    ScenePosList mPositionList;
    int mSpeed;
    int mZoomValue;
    int mZoomOldValue;
    sceneState mPlayingState;
private slots:
    void play();
    void open();
    void speedChanged(int d);
    void zoomChanged(int d);
};
#endif
