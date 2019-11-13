import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: addressesIdentificationId
    title: qsTr("Addresses Identification")
    nextPageComponent: "CommunicationTestPage.qml"
    previousPageComponent: "MdaTestConfirmationPage.qml"
	description: "All devices addresse needs to be specified here."
    Column {
		width: parent.width
		height: parent.heght
        LabeledTextInput {
			id: ip6100
            label.text: "6100 IP:"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})(\.0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})){3}$/ }
			textInput.inputMask: "000.000.000.000;_"
        }

        LabeledTextInput {
			id: ip6716
            label.text: "6716 IP:"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})(\.0*(2(5[0-5]|[0-4]\d)|1?\d{1,2})){3}$/ }
			textInput.inputMask: "000.000.000.000;_"
        }

		LabeledTextInput {
			id: fc3416_6716
            label.text: "3416 connected to the 6716 fc no:"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /[1-4]{1}$/ }
			textInput.inputMask: "0;_"
        }

		LabeledTextInput {
			id: fc3416_t028
            label.text: "3416 connected to the t028 fc no:"
			holderWidth : 400
			textInput.validator: RegExpValidator { regExp: /[1-4]{1}$/ }
			textInput.inputMask: "0;_"
        }
		Row {
			height: 45
			width: parent.width
			PlaceHolder {
				height: parent.height
				width: 200
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
	}

	Connections {
		target: dataInterface
		onConnectionStatus: ready = true;//status
	}

	Component.onCompleted: {
		ip6100.textInput.text = appSettings.get("ip6100", "192.168.168.000")
		ip6716.textInput.text = appSettings.get("ip6716", "192.168.168.000")
		fc3416_6716.textInput.text = appSettings.get("fc3416_6716", "1")
		fc3416_t028.textInput.text = appSettings.get("fc3416_t028", "1")
	}

	Component.onDestruction: {
		appSettings.set("ip6100", ip6100.textInput.text)
		appSettings.set("ip6716", ip6716.textInput.text)
		appSettings.set("fc3416_6716", fc3416_6716.textInput.text);
		appSettings.set("fc3416_t028", fc3416_t028.textInput.text);
	}
}
