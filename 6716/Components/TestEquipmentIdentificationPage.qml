import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testEquipmentIdentificationPageId
    title: qsTr("Test Equipment Identification")
    nextPageComponent: "TestSelectionPage.qml"
    previousPageComponent: "UutIdentificationPage.qml"
    ready: true
	description: "This page contains information details about test equipment."
    Column {
		width: parent.width
		height: parent.heght
        LabeledTextField {
			id: serialNumber_t028
            label.text: "T028 serial number:"
			textField.readOnly: true
			holderWidth : 400
        }

		LabeledTextField {
			id: versionId_t028
            label.text: "T028 version:"
			textField.readOnly: true
			holderWidth : 400
        }

		LabeledTextField {
			id: serialNumber_3416_t028
            label.text: "3416 wired to T028 serial number:"
			textField.readOnly: true
			holderWidth : 400
        }

		LabeledTextField {
			id: versionId_3416_t028
            label.text: "3416 wired to T028 version:"
			textField.readOnly: true
			holderWidth : 400
        }

        LabeledTextField {
			id: serialNumber_3416_6716
            label.text: "3416 wired to 6716 serial number:"
			textField.readOnly: true
			holderWidth : 400
        }

		LabeledTextField {
			id: versionId_3416_6716
            label.text: "3416 wired to 6716 version:"
			textField.readOnly: true
			holderWidth : 400
        }

	    LabeledTextField {
			id: testingVersionId
            label.text: "Testing software version"
			textField.text: "0.9"
			textField.readOnly: true
			holderWidth : 400
        }

		Connections {
			target: dataInterface
			onSnT028: serialNumber_t028.textField.text = value
			onSn3416_T028: serialNumber_3416_t028.textField.text = value
			onSn3416_6716: serialNumber_3416_6716.textField.text = value
			onVersionT028: versionId_t028.textField.text = value
			onVersion3416_T028: versionId_3416_t028.textField.text = value
			onVersion3416_6716: versionId_3416_6716.textField.text = value
		}

		Component.onCompleted: dataInterface.requestTestEquipmentData()
    }
}
