#include "../../src/nodeqml/engine.h"

#include <QCoreApplication>
#include <QQmlEngine>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QScopedPointer<QQmlEngine> engine(new QQmlEngine());
    QScopedPointer<NodeQml::Engine> node(new NodeQml::Engine(engine.data()));

    return a.exec();
}
