#include <QtWidgets>

#include <math.h>

#include "mouse.h"

typedef QList<QPointF> posList;
typedef QMap<QString, posList> ScenePosList;
typedef QMapIterator<QString, posList> ScenePosListIt;

#include <iostream>

static const int MouseCount = 7;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ScenePosList theScenePosList;
QStringList header;
    QFile Fin("./log.txt");
    if(Fin.exists())
    {
        Fin.open(QIODevice::ReadOnly | QIODevice::Text);

        QTextStream in(&Fin);

        QString firstLine = in.readLine();

        // for vle, solution + simple= split avec tab

        QRegExp rx("\\s+\"([^\":]+:[^\":]+)\"");
        //QStringList header;
        int pos = 0;

        while ((pos = rx.indexIn(firstLine, pos)) != -1) {
             header << rx.cap(1);
             pos += rx.matchedLength();
        }

        for (int i = 0; i < header.size() - 1; ++i) {
            std::cout << "-->" <<(header.at(i)).toLocal8Bit().constData() << "<--" <<std::endl;
        }

        //QStringList header = firstLine.split(":");

        //for (int i = 0; i < header.size() -1 ; ++i) {
        //    theScenePosList[header.at(i)] = posList();
        //}

        for (int i = 0; i < header.size() ; ++i) {
            theScenePosList[header.at(i)] = posList();
        }

        std::cout << header.size() - 1 << "::::::" << std::endl;

        ScenePosListIt i(theScenePosList);
        while (i.hasNext()) {
            i.next();
            std::cout << "---------"<<i.key().toLocal8Bit().constData() << ":" << std::endl;
       }

        std::cout << "ici" << std::endl;

        //while(!(in.atEnd())) {
        //    std::cout << "la" << std::endl;
        //    QString line = in.readLine();
        //    std::cout << line.toLocal8Bit().constData() << std::endl;
        //    QStringList fields = line.split(":");
        //    (theScenePosList[fields.at(1)]).push_back(QPointF(fields.at(2).toDouble(), fields.at(3).toDouble()));
        //    std::cout << fields.at(0).toLocal8Bit().constData() << "--->" << fields.at(1).toLocal8Bit().constData() << std::endl;
        //}

        while(!(in.atEnd())) {
           std::cout << "la" << std::endl;
           QString line = in.readLine();
           std::cout << line.toLocal8Bit().constData() << std::endl;
           QStringList fields = line.split("\t");

           QRegExp rx("[;\()]");
           for (int i = 1  ; i < fields.size() ; ++i) {
               //int pos = rx.indexIn(fields.at(i));
               //QString x = rx.cap(1); // "189"
               //QString y = rx.cap(2);
               QStringList xy = fields.at(i).split(rx, QString::SkipEmptyParts);
               QString x = xy.at(0); // "189"
               QString y = xy.at(1);
               std::cout << fields.at(i).toLocal8Bit().constData()<<"=X="<< x.toLocal8Bit().constData()<< "&&&&"<< y.toLocal8Bit().constData() << std::endl;
               theScenePosList[header.at(i - 1)].push_back(QPointF(x.toDouble(), y.toDouble()));
           }

         }
                //    (theScenePosList[fields.at(1)]).push_back(QPointF(fields.at(2).toDouble(), fields.at(3).toDouble()));
                //    std::cout << fields.at(0).toLocal8Bit().constData() << "--->" << fields.at(1).toLocal8Bit().constData() << std::endl;
                //}


        i.toFront();
        while (i.hasNext()) {
            i.next();
            std::cout << i.value().size()<< "::::" << std::endl;
        }
    }


    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
//! [0]

//! [1]
    QGraphicsScene scene;
    scene.setSceneRect(-300, -300, 600, 600);
//! [1] //! [2]
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);
//! [2]

//! [3]
  //  for (int i = 0; i < MouseCount; ++i) {
  //      QString mouseName = "Mouse_" + QString::number(i);
    for (int i = 0; i < header.size() ; ++i) {
        QString mouseName = header.at(i);

        Mouse *mouse;
        if(!Fin.exists()) {
            mouse = new Mouse;
        } else {
            std::cout <<"----------------"<<theScenePosList[mouseName].size()<<std::endl;
            mouse = new Mouse(&(theScenePosList[mouseName]));
        }
        mouse->setPos(::sin((i * 6.28) / MouseCount) * 200,
                      ::cos((i * 6.28) / MouseCount) * 200);

        mouse->setData(0,QVariant(mouseName));
        scene.addItem(mouse);
        std::cout << mouseName.toLocal8Bit().constData() <<  ":";
    }
    std::cout << std::endl;
//! [3]

//! [4]
    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(QPixmap(":images/cheese.jpg"));
//! [4] //! [5]
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view.setDragMode(QGraphicsView::ScrollHandDrag);
//! [5] //! [6]
    view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Colliding Mice"));
    view.resize(400, 300);
    view.show();

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    timer.start(1000 / 33);

    return app.exec();

}
