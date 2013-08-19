#include <QtWidgets>

#include <math.h>

#include <Mouse.hpp>

typedef QList<QPointF> posList;
typedef QMap<QString, posList> ScenePosList;
typedef QMapIterator<QString, posList> ScenePosListIt;

#include <iostream>

#include <MainWindow.hpp>

static const int MouseCount = 7;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mw;
    mw.show();
    return app.exec();
}
