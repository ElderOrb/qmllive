#include <QGuiApplication>
#include <QQmlApplicationEngine>

#ifdef QT_QMLLIVE
#include "livenodeengine.h"
#include "remotereceiver.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

#ifdef QT_QMLLIVE
    qDebug() << "app.applicationDirPath(): " << app.applicationDirPath();
    qDebug() << "writable data location: " << QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    auto writeLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    auto qmlFile = "main.qml";

    LiveNodeEngine node;
    // Let QmlLive know your runtime
    node.setQmlEngine(&engine);
    // Allow it to display QML components with non-QQuickWindow root object
    QQuickView fallbackView(&engine, 0);
    node.setFallbackView(&fallbackView);

    // Tell it where file updates should be stored relative to
#ifdef Q_OS_ANDROID
    // QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString workspaceRoot = writeLocation;
    qDebug() << "workspaceRoot: " << workspaceRoot;
#else
    // QString workspaceRoot = SRCDIR;
    QString workspaceRoot = writeLocation;
    qDebug() << "workspaceRoot: " << workspaceRoot;
#endif //
    node.setWorkspace(workspaceRoot,
                      LiveNodeEngine::AllowUpdates | LiveNodeEngine::UpdatesAsOverlay);
    // Listen to IPC call from remote QmlLive Bench
    RemoteReceiver receiver;
    QObject::connect(&receiver, &RemoteReceiver::activateDocument, [&](const LiveDocument& document) {
        qDebug() << "** document activated: " << document.relativeFilePath();
    });

    QObject::connect(&receiver, &RemoteReceiver::updateDocument, [&](const LiveDocument &document, const QByteArray &content) {
        qDebug() << "** document updated: " << document.relativeFilePath();
    });

    receiver.registerNode(&node);
    receiver.listen(10234);
    QQuickWindow* window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
    // Advanced use: let it know the initially loaded QML component (do this
    // only after registering to receiver!)
    QList<QQmlError> warnings;
    node.usePreloadedDocument(qmlFile, window, warnings);

    qDebug() << "QMLLIVE mode: listening " << 10234;
#endif
    return app.exec();
}
