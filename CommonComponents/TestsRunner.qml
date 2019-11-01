import QtQuick 2.4
import QtQuick.Controls 2.4

Column {
	signal runClicked()
	property alias runEnabled : runTestsButtonId.enabled
	property string runningTest : ""
	width: parent.width
	height: parent.height
	Row {
		width: parent.width
		height: parent.height - 90
		ScrollView {
			id: scrollViewId
			width: parent.width * (2/3)
			height: parent.height
			TextArea {
				id: textAreaId
				text: "Logs will be put here..."
				readOnly: true
				font.family: "Helvetica"
				font.pointSize: 10
				Connections {
					target: dataObject
					onLogMsg: textAreaId.append(msg)
				}
			}
		}
		Column {
			width: parent.width * (1/3)
			height: parent.height
			Label {
				width: parent.width
				text: runningTest
				font.family: "Helvetica"
				font.pointSize: 10
			}
		}
	}
	Column {
		width: parent.width
		height: 90
		Row {
			width: parent.width
			height: parent.height/2
			MyButton {
				id: clearButtonId
				width: parent.width/2
				height: parent.height
				text: "Clear"
				onClicked: textAreaId.text = ""
			}
			MyButton {
				width: parent.width/2
				height: parent.height
				text: "Copy"
				onClicked: clipboard.setText(textAreaId.text)
			}
		}
		MyButton {
			id: runTestsButtonId
			width: parent.width
			height: parent.height/2
			text: "Run tests"
			onClicked: {
				clearButtonId.clicked()
				runClicked()
			}
		}
	}
}