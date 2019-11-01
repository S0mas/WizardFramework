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
            label.text: "T028 serial number:"
			textField.text: dataObject.getSnT028()
			textField.readOnly: true
			holderWidth : 300
        }

		LabeledTextField {
            label.text: "T028 version:"
			textField.text: dataObject.getVersionT028()
			textField.readOnly: true
			holderWidth : 300
        }

		LabeledTextField {
            label.text: "3416 wired to T028 serial number:"
			textField.text: dataObject.getSn3416_T028()
			textField.readOnly: true
			holderWidth : 300
        }

		LabeledTextField {
            label.text: "3416 wired to T028 version:"
			textField.text: dataObject.getVersion3416_T028()
			textField.readOnly: true
			holderWidth : 300
        }

        LabeledTextField {
            label.text: "3416 wired to 6716 serial number:"
			textField.text: dataObject.getSn3416_6716()
			textField.readOnly: true
			holderWidth : 300
        }

		LabeledTextField {
            label.text: "3416 wired to 6716 version:"
			textField.text: dataObject.getVersion3416_6716()
			textField.readOnly: true
			holderWidth : 300
        }

	    LabeledTextField {
            label.text: "Testing software version"
			textField.text: "0.9"
			textField.readOnly: true
			holderWidth : 300
        }
    }
}
