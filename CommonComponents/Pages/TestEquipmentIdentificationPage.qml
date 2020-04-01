import QtQuick 2.12
import QtQuick.Controls 2.12
import "../../CommonComponents"

MyPage {
    id: testEquipmentIdentificationPageId
    title: qsTr("Test Equipment Identification")
    nextPageComponent: "qrc:/CommonComponents/Pages/TestSelectionPage.qml"
    previousPageComponent: "qrc:/CommonComponents/Pages/UutIdentificationPage.qml"
    ready: true
	description: "This page contains the information details about the test equipment."
    content: ScrollList {
		anchors.fill: parent
		model: testEquipmentIdentificationPageContent.namesIds()
		delegate: LabeledTextField {
			background: Rectangle {}
			anchors.horizontalCenter: parent.horizontalCenter
			title: testEquipmentIdentificationPageContent.description(modelData)
			textField.text: testEquipmentIdentificationPageContent.load(modelData)
			textField.enabled: !testEquipmentIdentificationPageContent.readOnly(modelData)
		}
	}
}
