import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testValidatorPageId
	width: parent.width
	height: parent.height
    title: qsTr("Test Validation")
    nextPageComponent: "TestCompletitionPage.qml"
    previousPageComponent: "TestSelectionPage.qml"
    ready: false
	description: "Information from tests run will be available here"
	TestsRunner {
		id: testsRunnerId
		runningTest: "None"
		onRunClicked: {
			runningTest = "None"
			runEnabled = false
			isPossibleToGoBack = false
			clearFinishedTestsList()
			dataInterface.startTests()
		}
	}

	Dialog {
		id: userActionDialog
		x: Math.round((parent.width - width) / 2)
		y: Math.round((parent.height - height) / 2)
		title: "Action required!"
		standardButtons: Dialog.Ok
		contentItem: Rectangle {
			implicitWidth: 400
			implicitHeight: 100
			Text {
				id: dialogTextId
				font.family: "Helvetica"
				font.pointSize: 12
				anchors.centerIn: parent
			}
		}
		onAccepted: dataInterface.continueAction()
	}

	Connections {
		target: dataInterface
		property var locale: Qt.locale()
		property date currentDate: new Date()
		property string dateString
		onTestsDone: {
			ready = true
			isPossibleToGoBack = true
			testsRunnerId.runEnabled = true
			testsRunnerId.runningTest = "all tests finished"
			dateString = currentDate.toLocaleDateString();
			console.log(Date.fromLocaleDateString(dateString));
			//saveFile("logs_Last.txt", testsRunnerId.text)
		}
		onAskUserAction: {
			dialogTextId.text = str
			userActionDialog.open()
		}
	}

	function saveFile(fileUrl, text) {
		var request = new XMLHttpRequest();
		request.open("PUT", fileUrl, false);
		request.send(text);
		return request.status;
	}

	Connections {
		target: testsModel
		onTestRunned: {
			testsRunnerId.runningTest = name
		}
		onTestFinished: {
			testsRunnerId.addFinishedTest(name, result)
		}
	}
}
