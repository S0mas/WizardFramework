import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

GroupBox {
	property var testsSelectionModelObject
	ColumnLayout {
		anchors.fill: parent
		ScrollList {
			Layout.minimumHeight: parent.height - buttonsRowId.height
			Layout.minimumWidth: parent.width
			Layout.fillWidth: true
			Layout.fillHeight: true
			model: testsSelectionModelObject.count()
			delegate: CheckBox { 
				height: 25
				Layout.fillWidth: true
				text: testsSelectionModelObject.getTestName(index)
				checked: testsSelectionModelObject.getTestShouldBeRun(index)
				font.pointSize: 12
				onClicked: testsSelectionModelObject.setTestShouldBeRun(index, checked)
				Connections {
					target: testsSelectionModelObject
					onTestShouldBeRunChanged: if(index == id) checked = value
				}
			}
		}
		RowLayout {
			id:	buttonsRowId
			Layout.minimumHeight: 30
			Layout.minimumWidth: 340
			spacing: 40
			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 150
				text: "Select all"
				onClicked: testsSelectionModelObject.setAllTestsShouldBeRun(true)
			}
			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 150
				text: "Deselect all"
				onClicked: testsSelectionModelObject.setAllTestsShouldBeRun(false)
			}
		}
	}
}
