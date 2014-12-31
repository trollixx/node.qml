#ifndef ENGINE_H
#define ENGINE_H

#include "nodeqml_global.h"

#include <QJSValue>
#include <QObject>

class QJSEngine;

namespace NodeQml {

class EnginePrivate;

class NODEQMLSHARED_EXPORT Engine : public QObject
{
    Q_OBJECT
public:

    explicit Engine(QJSEngine *jsEngine, QObject *parent = nullptr);

    QJSValue require(const QString &id);
    /// TODO: QJSValue evaluate(const QString &code);

    bool hasException() const;

signals:
    void quit(int returnCode = 0);

private:
    EnginePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Engine)
};

}

#endif // ENGINE_H
