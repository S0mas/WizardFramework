import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testValidatorPageId
	width: parent.width
	height: parent.height
    title: qsTr("Test Validation")
    nextPageComponent: "qrc:/CommonComponents/Pages/TestCompletitionPage.qml"
    previousPageComponent: "qrc:/CommonComponents/Pages/TestSelectionPage.qml"
    ready: true
	processing: testsRunnerId.isRunning
	description: "Information from the tests run will be available here"
	onCancel: mainTestsController.abort()

	content: TestsRunner {
		id: testsRunnerId
		anchors.fill: parent
		anchors.centerIn: parent
		testsController: mainTestsController
		summaryButtonVisible: true
	}

	function saveFile(fileUrl, text) {
		var request = new XMLHttpRequest();
		request.open("PUT", fileUrl, false);
		request.send(text);
		return request.status;
	}
}
