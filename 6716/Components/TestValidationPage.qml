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
			dataObject.startTests()
		}
	}

	Connections {
		target: dataObject
		onTestsDone: {
			ready = true
			isPossibleToGoBack = true
			testsRunnerId.runEnabled = true
			testsRunnerId.runningTest = "all tests finished"
		}
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
