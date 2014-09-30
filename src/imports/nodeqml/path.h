#ifndef PATH_H
#define PATH_H

#include "coremodule.h"

class Path : public CoreModule
{
    Q_OBJECT
    Q_PROPERTY(QString sep READ sep CONSTANT)
    Q_PROPERTY(QString delimiter READ delimiter CONSTANT)
public:
    explicit Path(QJSEngine *jsEngine, QObject *parent = nullptr);

    Q_INVOKABLE QString normalize(const QString &p) const;
    /// TODO: path.join([path1], [path2], [...])
    /// TODO: path.resolve([from ...], to)
    /// TODO: path.relative(from, to)
    Q_INVOKABLE QString dirname(const QString &p) const;
    Q_INVOKABLE QString basename(const QString &p, const QString &ext = QString()) const;
    Q_INVOKABLE QString extname(const QString &p) const;

private:
    QString sep() const;
    QString delimiter() const;
};

#endif // PATH_H
