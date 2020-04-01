import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

Item {
    property var clipboardObject
    function addLog(log) {
        textAreaId.append(log)
    }
    function clear() {
        textAreaId.text = ""
    }
    function copy() {
        clipboardObject.setText(textAreaId.text)
    }
    function save() {
        saveFileDialog.open();
    }
    id: root
    ColumnLayout {
        anchors.fill: parent
        ScrollView {
            Layout.fillHeight: true
            Layout.maximumHeight: parent.height - buttonsRowId.height
            Layout.fillWidth: true
            ScrollBar.vertical.policy: contentHeight > height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            ScrollBar.horizontal.policy: contentWidth > width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
            TextArea {
                id: textAreaId
                anchors.fill: parent
                readOnly: true
                font.pointSize: 10
                background: Rectangle {
                    anchors.fill: parent
                    border.color: "black"
                }
            }
        }
        Row {
            id: buttonsRowId
            Layout.fillWidth: true
            height: 25
            spacing: 40
            layoutDirection: Qt.RightToLeft
            MyButton {
                Layout.fillHeight: true
                width: 100
                text: "Clear"
                onClicked: root.clear()
            }
            MyButton {
                Layout.fillHeight: true
                width: 100
                text: "Copy"
                onClicked: root.copy()
            }
            MyButton {
                Layout.fillHeight: true
                width: 100
                text: "Save"
                onClicked: root.save()
            }
        }
    }

    function saveFile(fileUrl, text) {
        var request = new XMLHttpRequest();
        request.open("PUT", fileUrl, false);
        request.send(text);
        return request.status;
    }

    FileDialog {
        id: saveFileDialog
        selectExisting: false
        nameFilters: ["Text files (*.txt)", "All files (*)"]
        onAccepted: saveFile(saveFileDialog.fileUrl, textAreaId.text)
    }
}