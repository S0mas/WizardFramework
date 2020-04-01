import QtQuick 2.12
import QtQuick.Controls 2.12
import "../../CommonComponents"

MyPage {
    id: addressesIdentificationId
    title: qsTr("Addresses Identification")
    nextPageComponent: "qrc:/CommonComponents/Pages/CommunicationTestPage.qml"
    previousPageComponent: "qrc:/CommonComponents/Pages/UserIdentificationPage.qml"
	description: "All devices connection details needs to be specified here."
	ready: devicesController.connectionStatus
    content: Column {
		anchors.fill: parent
		spacing: 30
        ScrollList {
			width: parent.width
			model: addressesIdentificationPageContent.namesIds()
			delegate: LabeledTextInput {
				anchors.horizontalCenter: parent.horizontalCenter
				title: addressesIdentificationPageContent.description(modelData)
				textInput.text: addressesIdentificationPageContent.load(modelData)
				textInput.inputMask: addressesIdentificationPageContent.mask(modelData)
				textInput.color: addressesIdentificationPageContent.validate(modelData, textInput.text) ? "black" : "red"
				textInput.enabled: !addressesIdentificationPageContent.readOnly(modelData)
				textInput.onTextEdited: {
					addressesIdentificationPageContent.setTemporary(modelData, textInput.text)
					connectButtonId.enabled = addressesIdentificationPageContent.allValid()
				}
			}
		}
		MyButton {
			id: connectButtonId
			anchors.horizontalCenter: parent.horizontalCenter
			width: 150
			text: enabled ? "Connect" : "Connecting"
			onClicked: {
				enabled = false
				addressesIdentificationPageContent.setAll()
				addressesIdentificationPageContent.saveAll()
				devicesController.connectDevices()
			}
			Connections {
				target: devicesController
				onConnectionProcedureCompleted: connectButtonId.enabled = true
			}
		}

		LogsArea {
			id: logsArea
			clipboardObject: clipboard
			height: 300
			width: parent.width
			Connections {
				target: devicesController
				onLogMsg: logsArea.addLog(msg)
				onReportError: logsArea.addLog(errorMsg)
			}
		}
	}
}

