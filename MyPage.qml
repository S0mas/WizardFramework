import QtQuick 2.13
import QtQuick.Controls 2.12

Page {
    property string nextPageComponent: ""
    property string previousPageComponent: ""
    property bool ready: false
    signal handlerLoader(string name)

    width: parent.width
    height: parent.height

    header: Text {
        text: title
    }

    footer: Row {
        height: 40
        RoundButton {
            text: nextPageComponent == "" ? "Finish" : "Cancel"
            onClicked: Qt.callLater(Qt.quit)
        }
        RoundButton {
            text: "Back"
            onClicked: handlerLoader(previousPageComponent)
            enabled: previousPageComponent != ""
        }
        RoundButton {
            text: "Next"
            onClicked: handlerLoader(nextPageComponent)
            enabled: ready && nextPageComponent != ""
        }
    }
}
