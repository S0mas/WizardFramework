import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testSelectorPageId
    title: qsTr("Test Selection")
    nextPageComponent: "qrc:/CommonComponents/Pages/TestValidationPage.qml"
    previousPageComponent: "qrc:/CommonComponents/Pages/TestEquipmentIdentificationPage.qml"
    ready: true
	description: "The selected tests will be runned in next step."
	content: TestsSelection {
		anchors.fill: parent
		testsSelectionModelObject: testsModel
	}
}
