import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testSelectorPageId
    title: qsTr("Test Selection")
    nextPageComponent: "TestValidationPage.qml"
    previousPageComponent: "TestEquipmentIdentificationPage.qml"
    ready: true
	description: "Selected tests will be runned in next step."
	Column {
		width: parent.width
		height: parent.height - buttonsRowId.height
		ScrollList {
			id: scrollList
			width: parent.width
			height: parent.height - buttonsRowId.height
			model: testsModel
			delegate: CheckBox { 
				height: 25
				width: parent.width
				text: testsModel.getTestName(index)
				checked: testsModel.getTest(index).shouldBeRun
				font.family: "Helvetica"
				font.pointSize: 12
				onClicked: testsModel.getTest(index).shouldBeRun = checked
				Connections {
					target: testsModel.getTest(index)
					onShouldBeRunChanged: checked = testsModel.getTest(index).shouldBeRun
				}
			}
		}
		Row {
			id: buttonsRowId
			width: parent.width
			height: 45
			MyButton {
				width: parent.width/2
				height: parent.height
				text: "Select all"
				onClicked: testsModel.setAllTestsShouldBeRun(true)
			}
			MyButton {
				width: parent.width/2
				height: parent.height
				text: "Deselect all"
				onClicked: testsModel.setAllTestsShouldBeRun(false)
			}
		}
	}
}
