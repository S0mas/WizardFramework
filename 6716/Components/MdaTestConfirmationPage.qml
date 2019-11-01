import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: mdaTestConfirmationPageId
    title: qsTr("MDA Test Confirmation")
    nextPageComponent: mdaTestCheckBoxId.checked ? "CommunicationTestPage.qml" : "TestCompletitionPage.qml"
    previousPageComponent: "UserIdentificationPage.qml"
    ready: true
	description: "In case MDA test failed, you will not be able to continue production tests."
	Row {
		width: parent.width
		height: parent.height
		PlaceHolder {
			width: 150
			height: parent.height
		}
	    CheckBox {
			id: mdaTestCheckBoxId
			text: "Have MDA test passed?"
			onCheckedChanged: dataObject.setMdaTestPassed(checked)
			checked: dataObject.getMdaTestPassed()
			font.family: "Helvetica"
			font.pointSize: 12
		}
	}
}
