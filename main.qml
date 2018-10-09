import QtQuick 2.11
import QtQuick.Window 2.11
import '.' as Controls

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    // this one doesn't work in most cases (attempt to reload main.qml results in
    // '/data/data/org.qtproject.example.QmlLiveTest/files/main.qml:13: MyRectangle is not a type'
    MyRectangle {
        anchors.fill: parent
        color: 'green'
    }

    // this one always works (main.qml reloads just fine)
    Controls.MyRectangle {
        anchors.fill: parent
        color: 'green'
    }
}
