#include <MainWindow.hpp>
#include <QTimer>
#include <QFileDialog>
#include <QTextItem>
#include <QTextStream>
#include <QStringList>
#include <QTime>
#include <mouse.h>
#include <math.h>
#include <QDebug>
const int MainWindow::cMouseCount = 7;

MainWindow::MainWindow()
:mTimer(new QTimer()),mSpeed(30)
{
    createGraphicView();
    createToolBars();
    createStatusBar();
    setWindowTitle("myMices");
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
    mToolbar = QSharedPointer<QToolBar>(new QToolBar());
    mPlayAction = QSharedPointer<QAction>(new QAction("Play",mToolbar.data()));
    mOpenAction = QSharedPointer<QAction>(new QAction("Open",mToolbar.data()));
    mSpeedController = QSharedPointer<QSpinBox>(new QSpinBox(this));
    mSpeedController->setValue(mSpeed);
    mSpeedController->setMinimumWidth(110);
    mSpeedController->setSuffix("fps");
    mSpeedController->setMinimum(1);
    mSpeedController->setMaximum(1000);
    mSpeedController->setAlignment(Qt::AlignRight);
    mPlayAction->setEnabled(false);
    mToolbar->addAction(mOpenAction.data());
    mToolbar->addAction(mPlayAction.data());
    mToolbar->addWidget(mSpeedController.data());
    addToolBar(mToolbar.data());

    QObject::connect(mOpenAction.data(), SIGNAL(triggered()), this,
                     SLOT(open()));
    QObject::connect(mPlayAction.data(), SIGNAL(triggered()), this,
                     SLOT(play()));
    QObject::connect(mSpeedController.data(), SIGNAL(valueChanged(int)), this,
                     SLOT(speedChanged(int)));
}

void MainWindow::createStatusBar()
{
    mStatusBar = QSharedPointer<QStatusBar>(new QStatusBar());
    setStatusBar(mStatusBar.data());
    mStatusBar->showMessage(tr("Idle"));
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
    }
}

void MainWindow::play()
{
    QString state = mPlayAction->text();
    if (state == "Pause") {
        mTimer->stop();
        mPlayAction->setText("Resume");
        mStatusBar->showMessage(tr("Paused!"));
        mOpenAction->setEnabled(true);
    } else if (state == "Resume") {
        mTimer->start(1000.0/mSpeed);
        mPlayAction->setText("Pause");
        mStatusBar->showMessage(tr("Playing!"));
        mOpenAction->setEnabled(false);
    } else if (state == "Play") {
        mStatusBar->showMessage(tr("Playing!"));
        mPlayAction->setText("Pause");
        mTimer->start(1000.0/mSpeed);
        mOpenAction->setEnabled(false);
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
    mTimer->start(1000.0/mSpeed);
}

