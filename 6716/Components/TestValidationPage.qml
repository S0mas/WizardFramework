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
		summaryButtonVisible: true
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
		height: 100
		width: 400
		onRejected: dataInterface.continueAction()
		onAccepted: dataInterface.continueAction()
	}

	ConfirmationDialog {
		id: userConfirmationOrDeclineDialogId
		height: 100
		width: 400
		standardButtons: Dialog.Ok | Dialog.Cancel
		onRejected:  {
			dataInterface.inputUserDecision(false)
			dataInterface.continueAction()
		}
		onAccepted: {
			dataInterface.inputUserDecision(true)
			dataInterface.continueAction()
		}
	}

	ChannelsSelectionDialog {
		id: channelMaskDialogId
		standardButtons: Dialog.Ok
		defaultState: false
		onRejected: dataInterface.continueAction()
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
			if(actionType == 0) {
				userConfirmationDialogId.text = msg
				userConfirmationDialogId.title = msg
				userConfirmationDialogId.open()
			}
			else if(actionType == 1) {
				channelMaskDialogId.title = msg
				channelMaskDialogId.open()
				if(channelMaskDialogId.channelMask != "0000000000000000") // there was error reported by user 
					dataInterface.reportError(channelMaskDialogId.channelMask, 1)
			}
			else if(actionType == 2) {
				userConfirmationOrDeclineDialogId.text = msg
				userConfirmationOrDeclineDialogId.title = msg
				userConfirmationOrDeclineDialogId.open()
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
