import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: communicationTestPageId
	width: parent.width
	height: parent.height
    title: qsTr("I2C devices R/W tests")
    nextPageComponent: "UutIdentificationPage.qml"
    previousPageComponent: "AddressesIdentificationPage.qml"
    ready: true
	description: "This tests will check communication with i2c devices.\nAll tests needs to pass to continue further."

	TestsRunner {
		id: testsRunnerId
		width: parent.width
		height: parent.height
		onRunClicked: {
			runningTest = "None"
			runEnabled = false
			isPossibleToGoBack = false
			clearFinishedTestsList()
			dataInterface.startPreTests()
		}
	}

	Connections {
		target: dataInterface
		onTestsDone: {
			ready = preTestsModel.checkIfAllRunnedTestsPassed()
			isPossibleToGoBack = true
			testsRunnerId.runEnabled = true
			testsRunnerId.runningTest = "all tests finished"
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
		onTestFinished: {
			testsRunnerId.addFinishedTest(name, result)
		}
	}
}
