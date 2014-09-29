#ifndef DNS_H
#define DNS_H

#include <QDnsLookup>
#include <QJSValue>
#include <QObject>

class QJSEngine;

class Dns : public QObject
{
    Q_OBJECT
public:
    explicit Dns(QJSEngine *jsEngine, QObject *parent = 0);

    /// TODO: dns.lookup(domain, [family], callback)
    //Q_INVOKABLE void lookup(const QString &domain, QJSValue family, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolve(const QString &domain, QJSValue rrtype, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolve4(const QString &domain, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolve6(const QString &domain, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolveMx(const QString &domain, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolveTxt(const QString &domain, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolveSrv(const QString &domain, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolveNs(const QString &domain, QJSValue callback = QJSValue());
    Q_INVOKABLE void resolveCname(const QString &domain, QJSValue callback = QJSValue());
    /// TODO: dns.reverse(ip, callback)
    //Q_INVOKABLE void reverse(QString &ip, QJSValue callback);

private:
    void resolve(const QString &domain, QDnsLookup::Type type, QJSValue callback);

    QJSEngine *m_jsEngine = nullptr;
    QHash<QString, QDnsLookup::Type> m_recordTypes;
};

#endif // DNS_H
