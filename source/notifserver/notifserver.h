#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QDBusAbstractAdaptor>
#include <QDBusMessage>
#include <QDBusConnection>

// Implements org.freedesktop.Notifications on the session bus.
// Prints JSON lines to stdout for each Notify call so QML can parse them.

class NotifServer : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
    explicit NotifServer(QObject *parent = nullptr);
    bool registerOnBus();

public Q_SLOTS:
    // org.freedesktop.Notifications interface
    uint Notify(const QString &app_name,
                uint replaces_id,
                const QString &app_icon,
                const QString &summary,
                const QString &body,
                const QStringList &actions,
                const QVariantMap &hints,
                int expire_timeout);

    void CloseNotification(uint id);

    QStringList GetCapabilities();

    void GetServerInformation(QString &name, QString &vendor,
                              QString &version, QString &spec_version);

Q_SIGNALS:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &action_key);

private:
    uint _nextId = 1;
};
