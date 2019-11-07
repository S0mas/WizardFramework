import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: uutIdentificationPageId
    title: qsTr("UUT Identification")
    nextPageComponent: "TestEquipmentIdentificationPage.qml"
    previousPageComponent: "CommunicationTestPage.qml"
    ready: true
	description: "This page cotains information about unit under test. It is possible to updete this information (it will be saved on eeprom).\nTo update: check enable checkbox ---> change data that you want to update ---> click update. "
    Column {
		width: parent.width
		height: parent.height
        DataBaseField {
			id: subtypeLabelId
            label.text: "Subtype:"
            onSave: dataInterface.setSubtype(textField.text)
            onRefresh: dataInterface.requestUnitUnderTestData()
        }

        DataBaseField {
			id: snLabelId
            label.text: "Serial number"
            onSave: dataInterface.setSerialNumber(textField.text)
            onRefresh: dataInterface.requestUnitUnderTestData()
        }

		DataBaseField {
			id: firmwareLabelId
            label.text: "Firmware revision"
            onSave: dataInterface.setFirmwareRevision(textField.text)
            onRefresh: dataInterface.requestUnitUnderTestData()
        }

		LabeledTextField {
			id: driverLabelId
            label.text: "Driver revision"
			textField.enabled: false
        }

		Connections {
			target: dataInterface
			onSubtype: subtypeLabelId.textField.text = value
			onSerialNumber: snLabelId.textField.text = value
			onFirmwareRevision: firmwareLabelId.textField.text = value
			onDriverRevision: driverLabelId.textField.text = value
		}
    }
}
