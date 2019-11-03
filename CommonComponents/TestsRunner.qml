import QtQuick 2.4
import QtQuick.Controls 2.4

Row {
	signal runClicked()
	property alias runEnabled : runTestsButtonId.enabled
	function addFinishedTest(name, result) {
		runnedTestsListId.model.append({name: name, result: result})
	}
	function clearFinishedTestsList() {
		runnedTestsListId.model.clear()
	}
	property string runningTest : "None"
	width: parent.width
	height: parent.height
	Column {
		width: parent.width / 2
		height: parent.height
		ScrollView {
			id: scrollViewId
			width: parent.width
			height: parent.height - 90
			TextArea {
				id: textAreaId
				anchors.fill: parent
				background: Rectangle {
					border.width: 2
					border.color: "#000000"
					color: "#c3c3c3"
				}
				text: "Logs will be put here..."
				readOnly: true
				font.family: "Helvetica"
				font.pointSize: 12
				Connections {
					target: dataObject
					onLogMsg: textAreaId.append(msg)
				}
			}
		}
		Row {
			width: parent.width
			height: 45
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
			height: 45
			text: "Run tests"
			onClicked: {
				clearButtonId.clicked()
				runClicked()
			}
		}
	}
	Column {
		width: parent.width / 2
		height: parent.height
		ScrollList {
			id: runnedTestsListId
			height: parent.height - 30
			model: ListModel{}
			delegate: Row {
				height: 25
				width: parent.width
				Label {
					height: parent.height
					text: name
					color: result ? "green" : "red"
					font.family: "Helvetica"
					font.pointSize: 12
				}
			}
			background: Rectangle {
				border.width: 2
				border.color: "#000000"
				color: "#c3c3c3"
			}
		}
		Label {
			width: parent.width
			height: 30
			text: "Currently running test: " + runningTest
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignHCenter
			font.family: "Helvetica"
			font.pointSize: 12
			background: Rectangle {
				border.width: 2
				border.color: "#000000"
				color: "#c3c3c3"
			}
		}
	}
}	