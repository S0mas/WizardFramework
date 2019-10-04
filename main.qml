import QtQuick 2.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.12

ApplicationWindow {
    id:top
    width:640
    height:480
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
