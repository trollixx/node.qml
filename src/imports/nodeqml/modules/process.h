#ifndef PROCESS_H
#define PROCESS_H

#include "coremodule.h"

class Process : public CoreModule
{
    Q_OBJECT
public:
    explicit Process(QJSEngine *jsEngine, QObject *parent = nullptr);

signals:

public slots:

};

#endif // PROCESS_H
