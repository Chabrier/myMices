#include <MainWindow.hpp>
#include <QTimer>
#include <QFileDialog>
#include <QTextItem>
#include <QTextStream>
#include <QStringList>
#include <QTime>
#include <QLabel>
#include <Mouse.hpp>
#include <math.h>
#include <QDebug>
const int MainWindow::cMouseCount = 7;

MainWindow::MainWindow()
:mTimer(new QTimer()),
mSpeed(30),
mZoomValue(100),
mZoomOldValue(100),
mPlayingState(IDLE)
{
    createGraphicView();
    createToolBars();
    createStatusBar();
    setWindowTitle("myMices");
    setSceneState(IDLE);
}

void MainWindow::createGraphicView()
{
    mScene = QSharedPointer<QGraphicsScene>(new QGraphicsScene());
    mGraphicalView = QSharedPointer<QGraphicsView>(
                                              new QGraphicsView(mScene.data()));

    mScene->setSceneRect(-300, -300, 600, 600);
    mScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    mGraphicalView->setRenderHint(QPainter::Antialiasing);
    mGraphicalView->setBackgroundBrush(QPixmap(":/images/cheese.jpg"));

    mGraphicalView->setCacheMode(QGraphicsView::CacheBackground);
    mGraphicalView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    mGraphicalView->setDragMode(QGraphicsView::ScrollHandDrag);

    mGraphicalView->resize(400, 300);
    setCentralWidget(mGraphicalView.data());

    QObject::connect(mTimer.data(), SIGNAL(timeout()),
                     mScene.data(), SLOT(advance()));
}

void MainWindow::createToolBars()
{
    mToolbar = QSharedPointer<QToolBar>(new QToolBar(this));
    mPlayAction = QSharedPointer<QAction>(new QAction(mToolbar.data()));
    mPlayAction->setIcon(QIcon(":/images/play_black.png"));
    mOpenAction = QSharedPointer<QAction>(new QAction("Open",mToolbar.data()));
    mSpeedController = QSharedPointer<QSpinBox>(new QSpinBox(mToolbar.data()));
    mSpeedController->setValue(mSpeed);
    mSpeedController->setMinimumWidth(110);
    mSpeedController->setSuffix("fps");
    mSpeedController->setMinimum(1);
    mSpeedController->setMaximum(1000);
    mSpeedController->setAlignment(Qt::AlignRight);

    mZoomController = QSharedPointer<QSpinBox>(new QSpinBox(mToolbar.data()));
    mZoomController->setValue(mZoomValue);
    mZoomController->setMinimumWidth(110);
    mZoomController->setSuffix("%");
    mZoomController->setMinimum(1);
    mZoomController->setMaximum(1000);
    mZoomController->setAlignment(Qt::AlignRight);

    mToolbar->addAction(mOpenAction.data());
    mToolbar->addSeparator();
    mToolbar->addAction(mPlayAction.data());
    mToolbar->addSeparator();
    mToolbar->addWidget(new QLabel("Speed:", mToolbar.data()));
    mToolbar->addWidget(mSpeedController.data());
    mToolbar->addSeparator();
    mToolbar->addWidget(new QLabel("Zoom:", mToolbar.data()));
    mToolbar->addWidget(mZoomController.data());
    addToolBar(mToolbar.data());

    QObject::connect(mOpenAction.data(), SIGNAL(triggered()), this,
                     SLOT(open()));
    QObject::connect(mPlayAction.data(), SIGNAL(triggered()), this,
                     SLOT(play()));
    QObject::connect(mSpeedController.data(), SIGNAL(valueChanged(int)), this,
                     SLOT(speedChanged(int)));
    QObject::connect(mZoomController.data(), SIGNAL(valueChanged(int)), this,
                     SLOT(zoomChanged(int)));
}

void MainWindow::createStatusBar()
{
    mStatusBar = QSharedPointer<QStatusBar>(new QStatusBar());
    setStatusBar(mStatusBar.data());
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

        QObject::connect(mouse, SIGNAL(animationFinished()),
                     this, SLOT(animationFinished()));
    }
}

void MainWindow::play()
{
    QString state = mPlayAction->text();
    if (mPlayingState == PLAY) {
        mPlayingState = PAUSE;
        setSceneState(PAUSE);
    } else if (mPlayingState == PAUSE || mPlayingState == IDLE) {
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
        mStatusBar->showMessage(tr("Ready!"));
        mPlayAction->setText("Play");
        mPlayAction->setEnabled(true);
    }
}

void MainWindow::resetView()
{
    mPositionList.clear();
    mScene->clear();
}

void MainWindow::speedChanged(int d)
{
    mTimer->stop();
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
        case IDLE:
            mTimer->stop();
            mOpenAction->setEnabled(true);
            mPlayAction->setEnabled(false);
            mPlayAction->setText("Play");
            mPlayAction->setIcon(QIcon(":/images/play_black.png"));
            mStatusBar->showMessage(tr("Idle"));
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
    resetView();
    setSceneState(IDLE);
}

