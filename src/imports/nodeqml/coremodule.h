#ifndef COREMODULE_H
#define COREMODULE_H

#include <QObject>

class QJSEngine;

class CoreModule : public QObject
{
    Q_OBJECT
public:
    explicit CoreModule(QJSEngine *jsEngine, QObject *parent = nullptr);

protected:
    QJSEngine *jsEngine() const;

private:
    QJSEngine *m_jsEngine;
};

#endif // COREMODULE_H
