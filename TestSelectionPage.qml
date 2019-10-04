import QtQuick 2.13
import QtQuick.Controls 2.12

MyPage {
    id: testSelectorPageId
    title: qsTr("Test Selection")
    nextPageComponent: "TestValidationPage.qml"
    previousPageComponent: "TestEquipmentIdentificationPage.qml"
    ready: true
}
