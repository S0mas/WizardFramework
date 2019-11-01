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
            label.text: "Subtype:"
            onSave: dataObject.setSubtype(textField.text)
            onRefresh: textField.text = dataObject.getSubtype()
        }

        DataBaseField {
            label.text: "Serial number"
            onSave: dataObject.setSerialNumber(textField.text)
            onRefresh: textField.text = dataObject.getSerialNumber()
        }

		DataBaseField {
            label.text: "Firmware revision"
            onSave: dataObject.setFirmwareRevision(textField.text)
            onRefresh: textField.text = dataObject.getFirmwareRevision()
        }

		LabeledTextField {
            label.text: "Driver revision"
            textField.text: dataObject.getDriverRevision()
			textField.enabled: false
        }
    }
}
