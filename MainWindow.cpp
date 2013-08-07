#include <MainWindow.hpp>
#include <QTimer>
#include <QFileDialog>
#include <QTextItem>
#include <QTextStream>
#include <QStringList>
#include <QTime>
#include <QLabel>
#include <math.h>
#include <QDebug>
const int MainWindow::cMouseCount = 7;

MainWindow::MainWindow()
:mTimer(new QTimer(this)),
mSpeed(30),
mZoomValue(100),
mZoomOldValue(100),
mPlayingState(IDLE),
mFrameNumber(0),
mCurrentFrame(0)
{
    createGraphicView();
    createToolBars();
    createStatusBar();
    setWindowTitle("myMices");
    setSceneState(IDLE);
}

void MainWindow::createGraphicView()
{
    mScene = new QGraphicsScene(this);
    mGraphicalView = new QGraphicsView(mScene);

    mScene->setSceneRect(-300, -300, 600, 600);
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    mGraphicalView->setRenderHint(QPainter::Antialiasing);
    mGraphicalView->setBackgroundBrush(QPixmap(":/images/cheese.jpg"));

    mGraphicalView->setCacheMode(QGraphicsView::CacheBackground);
    mGraphicalView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    mGraphicalView->setDragMode(QGraphicsView::ScrollHandDrag);

    mGraphicalView->resize(400, 300);
    setCentralWidget(mGraphicalView);

    QObject::connect(mTimer, SIGNAL(timeout()),
                     mScene, SLOT(advance()));
    QObject::connect(mTimer, SIGNAL(timeout()),
                     this, SLOT(timerTic()));
}

void MainWindow::createToolBars()
{
    mToolbar = new QToolBar(this);
    mPlayAction = new QAction(mToolbar);
    mPlayAction->setIcon(QIcon(":/images/play_black.png"));
    mOpenAction = new QAction("Open",mToolbar);
    mSpeedController = new QSpinBox(mToolbar);
    mSpeedController->setValue(mSpeed);
    mSpeedController->setMinimumWidth(110);
    mSpeedController->setSuffix("fps");
    mSpeedController->setMinimum(1);
    mSpeedController->setMaximum(1000);
    mSpeedController->setAlignment(Qt::AlignRight);

    mZoomController = new QSpinBox(mToolbar);
    mZoomController->setValue(mZoomValue);
    mZoomController->setMinimumWidth(110);
    mZoomController->setSuffix("%");
    mZoomController->setMinimum(1);
    mZoomController->setMaximum(1000);
    mZoomController->setAlignment(Qt::AlignRight);

    mToolbar->addAction(mOpenAction);
    mToolbar->addSeparator();
    mToolbar->addAction(mPlayAction);
    mToolbar->addSeparator();
    mToolbar->addWidget(new QLabel("Speed:", mToolbar));
    mToolbar->addWidget(mSpeedController);
    mToolbar->addSeparator();
    mToolbar->addWidget(new QLabel("Zoom:", mToolbar));
    mToolbar->addWidget(mZoomController);
    addToolBar(mToolbar);

    QObject::connect(mOpenAction, SIGNAL(triggered()), this,
                     SLOT(open()));
    QObject::connect(mPlayAction, SIGNAL(triggered()), this,
                     SLOT(play()));
    QObject::connect(mSpeedController, SIGNAL(valueChanged(int)), this,
                     SLOT(speedChanged(int)));
    QObject::connect(mZoomController, SIGNAL(valueChanged(int)), this,
                     SLOT(zoomChanged(int)));
}

void MainWindow::createStatusBar()
{
    mStatusBar = new QStatusBar(this);
    mProgressBar = new QProgressBar(mStatusBar);
    mProgressBar->setRange(0,100);
    mProgressBar->setValue(0);
    mStatusBar->addPermanentWidget(mProgressBar);
    setStatusBar(mStatusBar);

    QObject::connect(this, SIGNAL(incrementProgressBar(int)), mProgressBar,
                 SLOT(setValue(int)));
}

void MainWindow::loadFile(QFile &file)
{
    QStringList header;
    if(file.exists()) {
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QTextStream in(&file);
        QString firstLine = in.readLine();

        QRegExp rx("\\s+\"([^\":]+:[^\":]+)\"");
        int pos = 0;
        while ((pos = rx.indexIn(firstLine, pos)) != -1) {
            header << rx.cap(1);
            pos += rx.matchedLength();
        }

        for (int i = 0; i < header.size() ; ++i) {
            mPositionList[header.at(i)] = posList();
        }

        ScenePosListIt i(mPositionList);
        while (i.hasNext()) {
            i.next();
        }

        while(!(in.atEnd())) {
            QString line = in.readLine();
            QStringList fields = line.split("\t");

            QRegExp rx("[;\()]");
            for (int i = 1  ; i < fields.size() ; ++i) {
                QStringList xy = fields.at(i).split(rx, QString::SkipEmptyParts);
                QString x = xy.at(0);
                QString y = xy.at(1);
                mPositionList[header.at(i - 1)].push_back(QPointF(x.toDouble(), y.toDouble()));
            }
            mFrameNumber = mPositionList[header.at(0)].size();
        }

        i.toFront();
        while (i.hasNext()) {
            i.next();
        }
    }


    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    for (int i = 0; i < header.size() ; ++i) {
        QString mouseName = header.at(i);

        Mouse *mouse;
        if(!file.exists()) {
            mouse = new Mouse;
        } else {
            mouse = new Mouse(&(mPositionList[mouseName]));
        }
        mouse->setPos(sin((i * 6.28) / cMouseCount) * 200,
        cos((i * 6.28) / cMouseCount) * 200);

        mouse->setData(0,QVariant(mouseName));
        mScene->addItem(mouse);
        mMices.push_back(mouse);
        QObject::connect(mouse, SIGNAL(animationFinished()),
                     this, SLOT(animationFinished()));
    }
}

void MainWindow::play()
{
    if (mPlayingState == RESET)
        mCurrentFrame = 0;

    if (mPlayingState == PLAY) {
        mPlayingState = PAUSE;
        setSceneState(PAUSE);
    } else if (mPlayingState == PAUSE
               || mPlayingState == IDLE
               || mPlayingState == RESET) {
        mPlayingState = PLAY;
        setSceneState(PLAY);
    }
}

void MainWindow::open()
{
    QString path = QFileDialog::getOpenFileName(this);

    if(path != "") {
        QFile logFile(path);
        resetView();
        loadFile(logFile);
        setSceneState(READY);
    }
}

void MainWindow::resetView()
{
    mPositionList.clear();
    mScene->clear();
    mMices.clear();
}

void MainWindow::speedChanged(int d)
{
    mSpeed = d;
    if (mPlayingState == PLAY) {
        mTimer->start(1000.0/mSpeed);
    }
}

void MainWindow::zoomChanged(int d)
{
    mGraphicalView->scale(d/(double)(mZoomOldValue),d/(double)(mZoomOldValue));
    mZoomOldValue = d;
}

void MainWindow::setSceneState(sceneState state)
{
    switch(state) {
        case READY:
            mStatusBar->showMessage(tr("Ready!"));
            mPlayAction->setText("Play");
            mPlayAction->setEnabled(true);
            mPlayAction->setIcon(QIcon(":/images/play_black.png"));
            mProgressBar->setValue(0);
        break;
        case IDLE:
            mTimer->stop();
            mOpenAction->setEnabled(true);
            mPlayAction->setEnabled(false);
            mPlayAction->setText("Play");
            mPlayAction->setIcon(QIcon(":/images/play_black.png"));
            mStatusBar->showMessage(tr("Idle"));
        break;
        case RESET:
            mTimer->stop();
            foreach(Mouse *m, mMices) {
                m->setIndex(0);
            }
            mOpenAction->setEnabled(true);
            mPlayAction->setIcon(QIcon(":/images/reset_black.png"));
            mStatusBar->showMessage(tr("Launch again?"));
        break;
        case PLAY:
            mTimer->start(1000.0/mSpeed);
            mPlayAction->setText("Pause");
            mPlayAction->setIcon(QIcon(":/images/pause_black.png"));
            mStatusBar->showMessage(tr("Playing!"));
            mOpenAction->setEnabled(false);
        break;
        case PAUSE:
            mTimer->stop();
            mPlayAction->setText("Resume");
            mStatusBar->showMessage(tr("Paused!"));
            mPlayAction->setIcon(QIcon(":/images/play_black.png"));
            mOpenAction->setEnabled(true);
        break;
    }
}

void MainWindow::animationFinished()
{
    mTimer->stop();
    mPlayingState = RESET;
    setSceneState(RESET);
}

void MainWindow::timerTic()
{
    ++mCurrentFrame;
    qDebug() << "[DEBUG]" <<  mCurrentFrame << "/" << mFrameNumber;
    emit incrementProgressBar(
                             (mCurrentFrame/(double)mFrameNumber) * 100);
}
