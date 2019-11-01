import QtQuick 2.4
import QtQuick.Controls 2.4

Page {
    property string nextPageComponent: ""
    property string previousPageComponent: ""
    property bool ready: false
	property bool isPossibleToGoBack: true
	property string description: "Test description"
    signal handlerLoader(string name)
	signal cancel
	signal back
	signal next
    width: parent.width
    height: parent.height

    header: Column {
		Image {
			width: 200
			height: 80
			source: "qrc:///images/bustec.jpg"
			anchors.horizontalCenter: parent.horizontalCenter
		}
		Text {
			text: title
			font.family: "Helvetica"
			font.pointSize: 24
		}
		Text {
			text: description
			font.family: "Helvetica"
			font.pointSize: 12
		}
		PlaceHolder {
			width: parent.width
			height: 45
		}
	}

    footer: Column {
		width: parent.width
		height: 65
		PlaceHolder {
			width: parent.width
			height: 20
		}
		Row {
			height: 45
			width: parent.width
			MyButton {
				width: parent.width/3
				height: parent.height
				text: nextPageComponent == "" ? "Finish" : "Cancel"
				onClicked: {
					cancel()
					Qt.callLater(Qt.quit)
				}
			}
			MyButton {
				width: parent.width/3
				height: parent.height
				text: "Back"
				onClicked: {
					back()
					handlerLoader(previousPageComponent)
				}
				enabled: isPossibleToGoBack && previousPageComponent != ""
			}
			MyButton {
				width: parent.width/3
				height: parent.height
				text: "Next"
				onClicked: {
					next()
					handlerLoader(nextPageComponent)
				}
				enabled: ready && nextPageComponent != ""
			}
		}
	}
}
