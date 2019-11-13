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

	ConfirmationDialog {
		id: userConfirmationDialogId
		onAccepted: dataInterface.continueAction()
	}

	ChannelsSelectionDialog {
		id: channelMaskDialogId
		standardButtons: Dialog.Ok
		defaultState: false
		onAccepted: dataInterface.continueAction()
	}

	Connections {
		target: dataInterface
		///property var locale: Qt.locale()
		//property date currentDate: new Date()
		//property string dateString
		onTestsDone: {
			ready = true
			isPossibleToGoBack = true
			testsRunnerId.runEnabled = true
			testsRunnerId.runningTest = "all tests finished"
			//dateString = currentDate.toLocaleDateString();
			//console.log(Date.fromLocaleDateString(dateString));
			//saveFile("logs_Last.txt", testsRunnerId.text)
			//saveFile("C:\\ksommerf\\WizardFramework", "text in file")
		}
		onAskUserAction: {
			dialogTextId.text = str
			if(actionType == 0)
				userConfirmationDialogId.open()
			if(actionType == 1){
				channelMaskDialogId.open()
				testsRunnerId.textAreaId.text += "Channels with faulty leds are:" + channelMaskDialogId + "\n"
			}
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
