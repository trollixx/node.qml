#include "dns.h"

/*#include <QDnsLookup>
#include <QHostAddress>
#include <QJSEngine>
#include <QLoggingCategory>
#include <QVariant>

#include <QDebug>

namespace {
const QLoggingCategory logCategory("nodeqml.dns");
}

Dns::Dns(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{
    /// TODO: Use meta enum
    m_recordTypes.insert(QStringLiteral("A"), QDnsLookup::A);
    m_recordTypes.insert(QStringLiteral("AAAA"), QDnsLookup::AAAA);
    m_recordTypes.insert(QStringLiteral("CNAME"), QDnsLookup::CNAME);
    m_recordTypes.insert(QStringLiteral("MX"), QDnsLookup::MX);
    m_recordTypes.insert(QStringLiteral("NS"), QDnsLookup::NS);
    m_recordTypes.insert(QStringLiteral("PTR"), QDnsLookup::PTR);
    m_recordTypes.insert(QStringLiteral("SRV"), QDnsLookup::SRV);
    m_recordTypes.insert(QStringLiteral("TXT"), QDnsLookup::TXT);
}*/

/*void Dns::lookup(const QString &domain, QJSValue family, QJSValue callback)
{
    int f = 0;
    if (callback.isCallable()) {
        f = family.toInt();
        if (f != 4 && f != 6) {
            qCWarning(logCategory, "Invalid family value.");
            return;
        }
    } else if (family.isCallable()) {
        callback = family;
    } else {
        qCWarning(logCategory, "No callback provided.");
        return;
    }

    QDnsLookup *dns = new QDnsLookup(this);
    dns->setName(domain);
    if (f == 4)
        dns->setType(QDnsLookup::A);
    else if (f == 6)
        dns->setType(QDnsLookup::AAAA);
    else
        ; /// TODO: QHostInfo::lookupHost()

    connect(dns, &QDnsLookup::finished, [dns, callback]() {
        QJSValueList args;
        if (dns->error() == QDnsLookup::NoError) {
            args << QJSValue::NullValue;
            qDebug() << "A/AAAA:" << dns->hostAddressRecords().size();
            qDebug() << "CNAME:" << dns->canonicalNameRecords().size();
            qDebug() << "MX:" << dns->mailExchangeRecords().size();
            qDebug() << "NS:" << dns->nameServerRecords().size();
            qDebug() << "PTR:" << dns->pointerRecords().size();
            qDebug() << "SRV:" << dns->serviceRecords().size();
            qDebug() << "TXT:" << dns->textRecords().size();
            qDebug() << dns->hostAddressRecords().size();
            const QDnsHostAddressRecord &record = dns->hostAddressRecords().first();
            args << record.value().toString();
            args << (record.value().protocol() == QAbstractSocket::IPv6Protocol ? 6 : 4);
            qDebug() << args.size();
        } else {
            args << QJSValue(dns->error());
        }
        dns->deleteLater();
        QJSValue(callback).call(args); /// FIXME: Ugly hack?
    });
    dns->lookup();
}*/

/*void Dns::resolve(const QString &domain, QJSValue rrtype, QJSValue callback)
{
    if (!callback.isCallable()) {
        if (rrtype.isCallable()) {
            callback = rrtype;
            rrtype = QStringLiteral("A");
        } else {
            qCWarning(logCategory, "No callback provided.");
        }
    }

    if (!m_recordTypes.contains(rrtype.toString())) {
        qCWarning(logCategory, "Invalid record type.");
        return;
    }

    resolve(domain, m_recordTypes.value(rrtype.toString()), callback);
}

void Dns::resolve4(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::A, callback);
}

void Dns::resolve6(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::AAAA, callback);
}

void Dns::resolveMx(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::MX, callback);
}

void Dns::resolveTxt(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::TXT, callback);
}

void Dns::resolveSrv(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::SRV, callback);
}

void Dns::resolveNs(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::NS, callback);
}

void Dns::resolveCname(const QString &domain, QJSValue callback)
{
    resolve(domain, QDnsLookup::CNAME, callback);
}

void Dns::reverse(const QString &ip, QJSValue callback)
{
    if (!callback.isCallable()) {
        qCWarning(logCategory, "No callback provided.");
        return;
    }
    int lookupId = QHostInfo::lookupHost(ip, this, SLOT(reverseLookupDone(QHostInfo)));
    m_lookupCallbacks.insert(lookupId, callback);
}

/// TODO: Return all records
void Dns::reverseLookupDone(QHostInfo hostInfo)
{
    if (!m_lookupCallbacks.contains(hostInfo.lookupId()))
        return;
    QJSValue callback = m_lookupCallbacks.value(hostInfo.lookupId());
    m_lookupCallbacks.remove(hostInfo.lookupId());

    if (hostInfo.error()) {
        /// TODO: Return Node's error code
        callback.call({createError(hostInfo.errorString(), hostInfo.error())});
        return;
    }

    QJSValue array = jsEngine()->newArray(1);
    array.setProperty(0, hostInfo.hostName());
    callback.call({QJSValue::NullValue, array});
}

void Dns::resolve(const QString &domain, QDnsLookup::Type type, QJSValue callback)
{
    QDnsLookup *dns = new QDnsLookup(type, domain, this);
    connect(dns, &QDnsLookup::finished, [this, dns, callback]() {
        QJSValueList args;

        if (dns->error() == QDnsLookup::NoError) {
            args << QJSValue::NullValue; // err

            QJSValue array = jsEngine()->newArray();

            switch (dns->type()) {
            case QDnsLookup::A:
            case QDnsLookup::AAAA:
                for (int i = 0; i < dns->hostAddressRecords().size(); ++i)
                    array.setProperty(i, dns->hostAddressRecords().at(i).value().toString());
                break;
            case QDnsLookup::CNAME:
                for (int i = 0; i < dns->canonicalNameRecords().size(); ++i)
                    array.setProperty(i, dns->canonicalNameRecords().at(i).value());
                break;
            case QDnsLookup::MX:
                for (int i = 0; i < dns->mailExchangeRecords().size(); ++i)
                    array.setProperty(i, dns->mailExchangeRecords().at(i).exchange());
                break;
            case QDnsLookup::NS:
                for (int i = 0; i < dns->nameServerRecords().size(); ++i)
                    array.setProperty(i, dns->nameServerRecords().at(i).value());
                break;
            case QDnsLookup::PTR:
                for (int i = 0; i < dns->pointerRecords().size(); ++i)
                    array.setProperty(i, dns->pointerRecords().at(i).value());
                break;
            case QDnsLookup::SRV:
                for (int i = 0; i < dns->serviceRecords().size(); ++i)
                    array.setProperty(i, dns->serviceRecords().at(i).target());
                break;
            case QDnsLookup::TXT: {
                int i = 0;
                foreach (const QDnsTextRecord &record, dns->textRecords()) {
                    foreach (const QString &txt, dns->textRecords().at(i).values())
                        array.setProperty(i++, txt);
                }
            }
                break;
            case QDnsLookup::ANY:
            default:
                break;
            }
            args << array;
        } else {
            /// TODO: Return Node's error code
            args << createError(dns->errorString(), dns->error());
        }
        dns->deleteLater();
        QJSValue(callback).call(args); /// FIXME: Ugly hack?
    });
    dns->lookup();
}
*/
