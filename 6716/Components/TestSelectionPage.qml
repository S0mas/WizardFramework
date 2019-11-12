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
	onNext: dataInterface.setStoreCalibrationDataToEeprom(saveEepromCheckId.checked)
	
	ChannelsSelectionDialog {
		id: channelMaskDialogId
		onAccepted: dataInterface.setChannelMask(channelMask)
	}
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
				checked: testsModel.getTestShouldBeRun(index)
				font.family: "Helvetica"
				font.pointSize: 12
				onClicked: testsModel.setTestShouldBeRun(index, checked)
				Connections {
					target: testsModel
					onTestShouldBeRunChanged: if(index == id) checked = value
				}
			}
		}
		Row {
			id: buttonsRowId
			width: parent.width
			height: 45
			MyButton {
				width: parent.width/4
				height: parent.height
				text: "Select all"
				onClicked: testsModel.setAllTestsShouldBeRun(true)
			}
			MyButton {
				width: parent.width/4
				height: parent.height
				text: "Deselect all"
				onClicked: testsModel.setAllTestsShouldBeRun(false)
			}
			MyButton {
				width: parent.width/4
				height: parent.height
				text: "Set channel mask"
				onClicked: channelMaskDialogId.open()
			}
			CheckBox {
				id: saveEepromCheckId
				width: parent.width/4
				height: parent.height
				text: "Enable eeprom writing"
				font.family: "Helvetica"
				font.pointSize: 12
				checked: true
			}
		}
	}
}
