#ifndef OS_H
#define OS_H

#include "coremodule.h"

class Os : public CoreModule
{
    Q_OBJECT
public:
    explicit Os(QJSEngine *jsEngine, QObject *parent = nullptr);

};

#endif // OS_H
