// notifserver — implements org.freedesktop.Notifications D-Bus service.
//
// Registers on the session bus as the system notification server.
// For each Notify call, emits a JSON line to stdout:
//   {"type":"notify","id":1,"app":"...","summary":"...","body":"...","timeout":5000}
//
// For CloseNotification, emits:
//   {"type":"close","id":1}
//
// QML NotificationService.qml reads this via Process + SplitParser.

#include "notifserver.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusMetaType>
#include <QDateTime>
#include <QTextStream>
#include <iostream>
#include <string>

static std::string escJson(const std::string &s) {
    std::string r;
    for (char c : s) {
        if (c == '"')       r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\r') r += "";
        else                r += c;
    }
    return r;
}

static std::string esc(const QString &s) { return escJson(s.toStdString()); }

// ── NotifServer ───────────────────────────────────────────────────────────────

NotifServer::NotifServer(QObject *parent) : QObject(parent) {}

bool NotifServer::registerOnBus() {
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        std::cerr << "[notifserver] Cannot connect to session D-Bus\n";
        return false;
    }

    if (!bus.registerObject("/org/freedesktop/Notifications", this,
                            QDBusConnection::ExportScriptableSlots |
                            QDBusConnection::ExportScriptableSignals)) {
        std::cerr << "[notifserver] registerObject failed: "
                  << bus.lastError().message().toStdString() << "\n";
        return false;
    }

    if (!bus.registerService("org.freedesktop.Notifications")) {
        std::cerr << "[notifserver] registerService failed (another notif server running?): "
                  << bus.lastError().message().toStdString() << "\n";
        return false;
    }

    return true;
}

uint NotifServer::Notify(const QString &app_name,
                          uint replaces_id,
                          const QString &app_icon,
                          const QString &summary,
                          const QString &body,
                          const QStringList &actions,
                          const QVariantMap &hints,
                          int expire_timeout) {
    (void)app_icon; (void)actions; (void)hints;

    uint id = (replaces_id > 0) ? replaces_id : _nextId++;

    // Build actions JSON array (pairs: action_key, label)
    std::string actJson = "[";
    for (int i = 0; i + 1 < actions.size(); i += 2) {
        if (i > 0) actJson += ",";
        actJson += "{\"key\":\"" + esc(actions[i]) + "\","
                   "\"label\":\"" + esc(actions[i+1]) + "\"}";
    }
    actJson += "]";

    std::cout << "{\"type\":\"notify\","
              << "\"id\":"          << id << ","
              << "\"app\":\""       << esc(app_name) << "\","
              << "\"summary\":\""   << esc(summary) << "\","
              << "\"body\":\""      << esc(body) << "\","
              << "\"actions\":"     << actJson << ","
              << "\"timeout\":"     << expire_timeout << "}\n";
    std::cout.flush();

    return id;
}

void NotifServer::CloseNotification(uint id) {
    std::cout << "{\"type\":\"close\",\"id\":" << id << "}\n";
    std::cout.flush();
    Q_EMIT NotificationClosed(id, 3); // reason 3 = closed by call
}

QStringList NotifServer::GetCapabilities() {
    return {"body", "actions", "persistence"};
}

void NotifServer::GetServerInformation(QString &name, QString &vendor,
                                        QString &version, QString &spec_version) {
    name         = "Aethera Shell Notifications";
    vendor       = "aethera";
    version      = "1.0";
    spec_version = "1.2";
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    NotifServer server;
    if (!server.registerOnBus())
        return 1;

    return app.exec();
}
