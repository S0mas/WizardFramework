import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: addressesIdentificationId
    title: qsTr("Addresses Identification")
    nextPageComponent: "CommunicationTestPage.qml"
    previousPageComponent: "MdaTestConfirmationPage.qml"
	ready: ip6100.textInput.acceptableInput && ip6716.textInput.acceptableInput && fc3416_6716.textInput.acceptableInput && fc3416_t028.textInput.acceptableInput
	description: "All devices addresse needs to be specified here."
    Column {
		width: parent.width
		height: parent.heght
        LabeledTextInput {
			id: ip6100
            label.text: "6100 IP"
			textInput.text: "192.168.168.126"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})(\.0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})){3}$/ }
			textInput.onTextChanged: ready = textInput.acceptableInput && ip6716.textInput.acceptableInput && fc3416_6716.textInput.acceptableInput && fc3416_t028.textInput.acceptableInput
			textInput.inputMask: "000.000.000.000;_"
        }

		LabeledTextInput {
			id: ip6716
            label.text: "6716 IP:"
			textInput.text: "192.168.168.147"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})(\.0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})){3}$/ }
			textInput.onTextChanged: ready = ip6100.textInput.acceptableInput && textInput.acceptableInput && fc3416_6716.textInput.acceptableInput && fc3416_t028.textInput.acceptableInput
			textInput.inputMask: "000.000.000.000;_"
        }

		LabeledTextInput {
			id: fc3416_6716
            label.text: "3416 connected to the 6716 fc no:"
			textInput.text: "2"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /[1-4]{1}$/ }
			textInput.onTextChanged: ready = ip6100.textInput.acceptableInput && ip6716.textInput.acceptableInput && textInput.acceptableInput && fc3416_t028.textInput.acceptableInput
			textInput.inputMask: "0;_"
        }

		LabeledTextInput {
			id: fc3416_t028
            label.text: "3416 connected to the t028 fc no:"
			textInput.text: "4"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /[1-4]{1}$/ }
			textInput.onTextChanged: ready = ip6100.textInput.acceptableInput && ip6716.textInput.acceptableInput && fc3416_6716.textInput.acceptableInput && textInput.acceptableInput
			textInput.inputMask: "0;_"
        }

		MyButton {
			text: "Connect"
			onClicked: {
				dataInterface.disconnectDevices()
				dataInterface.connectDevices(ip6100.textInput.text, ip6716.textInput.text, fc3416_6716.textInput.text, fc3416_t028.textInput.text)
				dataInterface.requestConnectionStatus()
			}
		}
	}

	Connections {
		target: dataInterface
		onConnectionStatus: ready = status
	}
}
