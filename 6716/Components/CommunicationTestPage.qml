import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: communicationTestPageId
	width: parent.width
	height: parent.height
    title: qsTr("I2C devices R/W tests")
    nextPageComponent: "UutIdentificationPage.qml"
    previousPageComponent: "MdaTestConfirmationPage.qml"
    ready: false
	description: "This tests will check communication with i2c devices.\nAll tests needs to pass to continue further."

	TestsRunner {
		id: testsRunnerId
		runningTest: "None"
		onRunClicked: {
			if(!ready){
				runEnabled = false
				isPossibleToGoBack = false;
				dataObject.startPreTests()
			}
		}
	}

	Connections {
		target: dataObject
		onTestsDone: {
			ready = preTestsModel.checkIfAllRunnedTestsPassed()
			isPossibleToGoBack = true;
			testsRunnerId.runEnabled = true
			if(!ready)
				nextPageComponent = ""
			else
				nextPageComponent = "UutIdentificationPage.qml"
		}
	}

	Connections {
		target: preTestsModel
		onTestRunned: {
			testsRunnerId.runningTest = name
		}
	}
}
