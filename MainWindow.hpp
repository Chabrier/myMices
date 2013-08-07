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
#include <QProgressBar>
#include <Mouse.hpp>

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
    typedef enum {IDLE,PLAY,PAUSE,RESET,READY} sceneState;

    void loadFile(QFile&);
    void resetView();
    void setSceneState(sceneState);
private:
    typedef QList<QPointF> posList;
    typedef QMap<QString, posList> ScenePosList;
    typedef QMapIterator<QString, posList> ScenePosListIt;
    static const int cMouseCount;

    QGraphicsScene  *mScene;
    QGraphicsView   *mGraphicalView;
    QToolBar        *mToolbar;
    QAction         *mPlayAction;
    QAction         *mOpenAction;
    QSpinBox        *mSpeedController;
    QSpinBox        *mZoomController;
    QTimer          *mTimer;
    QProgressBar    *mProgressBar;
    QStatusBar      *mStatusBar;
    QVector<Mouse*> mMices;

    ScenePosList mPositionList;
    int mSpeed;
    int mZoomValue;
    int mZoomOldValue;
    sceneState mPlayingState;
    int mFrameNumber;
    int mCurrentFrame;
private slots:
    void play();
    void open();
    void speedChanged(int d);
    void zoomChanged(int d);
    void animationFinished();
    void timerTic();
signals:
    void incrementProgressBar(int);
};
#endif
