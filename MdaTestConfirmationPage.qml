import QtQuick 2.13
import QtQuick.Controls 2.12

MyPage {
    id: mdaTestConfirmationPageId
    title: qsTr("MDA Test Confirmation")
    nextPageComponent: mdaTestCheckBoxId.checked ? "TestEquipmentIdentificationPage.qml" : "TestCompletitionPage.qml"
    previousPageComponent: "UutIdentificationPage.qml"
    ready: true

    CheckBox {
        id: mdaTestCheckBoxId
        text: "Have MDA test passed?"
        onCheckedChanged: dataObject.setMdaTestPassed(checked)
        checked: dataObject.getMdaTestPassed()
    }
}
