#include <nodeqml/engine.h>

#include <QtTest/QtTest>

class tst_node: public QObject
{
    Q_OBJECT
private slots:
    void run();
};

void tst_node::run()
{
    QVERIFY(true);
}

QTEST_MAIN(tst_node)
#include "tst_node.moc"
