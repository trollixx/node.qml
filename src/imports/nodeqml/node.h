#ifndef NODE_H
#define NODE_H

#include <QHash>
#include <QJSValue>
#include <QObject>

class QJSEngine;
class QQmlEngine;

class CoreModule;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QQmlEngine *qmlEngine, QJSEngine *jsEngine, QObject *parent = 0);

    Q_INVOKABLE QJSValue require(const QString &module);
signals:

public slots:

private:
    void setupCoreModules();

    QJSEngine *m_jsEngine;
    QQmlEngine *m_qmlEngine;
    QHash<QString, CoreModule *> m_coreModules;
};

#endif // NODE_H
