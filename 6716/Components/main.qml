import QtQuick 2.4
import QtQuick.Window 2.4
import QtQuick.Controls 2.4

ApplicationWindow {
    id:top
    width:1280
    height:780
    visible: true
    signal handlerLoader(string name)

    Loader {
        id: pageLoader
        source: "UserIdentificationPage.qml"
        anchors.fill: parent
    }

    Connections {
        target: pageLoader.item
        onHandlerLoader: pageLoader.source = name;
    }
}
