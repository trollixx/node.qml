#ifndef COREMODULE_H
#define COREMODULE_H

#include <QJSValue>
#include <QObject>

class QJSEngine;

class CoreModule : public QObject
{
    Q_OBJECT
public:
    explicit CoreModule(QJSEngine *jsEngine, QObject *parent = nullptr);

protected:
    QJSValue createError(const QString &message, int code = 0);
    QJSEngine *jsEngine() const;

private:
    QJSEngine *m_jsEngine;
};

#endif // COREMODULE_H
