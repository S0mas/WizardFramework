import QtQuick 2.13
import QtQuick.Controls 2.12

MyPage {
    id: testValidatorPageId
    title: qsTr("Test Validation")
    nextPageComponent: "TestCompletitionPage.qml"
    previousPageComponent: "TestSelectionPage.qml"
    ready: true
}
