import QtQuick 2.12
import QtQuick.Controls 2.12
import "../../CommonComponents"

MyPage {
    id: uutIdentificationPageId
    title: qsTr("UUT Identification")
    nextPageComponent: "qrc:/CommonComponents/Pages/TestEquipmentIdentificationPage.qml"
    previousPageComponent: "qrc:/CommonComponents/Pages/CommunicationTestPage.qml"
    ready: true
	description: "This page cotains information about unit under test. It is possible to updete this information (it will be saved on eeprom).\nTo update: mark the checkbox ---> change the data ---> click update. "
    content: ScrollList {
		anchors.fill: parent
		model: uutIdentificationPageContent.namesIds()
		delegate: DataBaseField {
			anchors.horizontalCenter: parent.horizontalCenter
			title: uutIdentificationPageContent.description(modelData)
			onSave: {
				uutIdentificationPageContent.setValue(modelData, textInput.text)
				uutIdentificationPageContent.save(modelData)
			}	
			onRefresh: textInput.text = uutIdentificationPageContent.load(modelData)
			textInput.text: uutIdentificationPageContent.load(modelData)
			textInput.inputMask: uutIdentificationPageContent.mask(modelData)
			textInput.color: uutIdentificationPageContent.validate(modelData, textInput.text) ? "black" : "red"
			textInput.enabled: !uutIdentificationPageContent.readOnly(modelData)
		}
	}
}
