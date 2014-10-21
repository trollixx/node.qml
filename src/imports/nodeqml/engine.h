#ifndef ENGINE_H
#define ENGINE_H

#include <QJSValue>
#include <QObject>

class QQmlEngine;

namespace NodeQml {

class EnginePrivate;

class Engine : public QObject
{
    Q_OBJECT
public:

    explicit Engine(QQmlEngine *qmlEngine, QObject *parent = nullptr);

    QJSValue require(const QString &id);
    /// TODO: QJSValue evaluate(const QString &code);

private:
    EnginePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Engine)
};

}

#endif // ENGINE_H
