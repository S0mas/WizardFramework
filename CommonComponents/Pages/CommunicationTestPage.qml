import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.11
import "../../CommonComponents"

MyPage {
    id: communicationTestPageId
    title: qsTr("I2C devices R/W tests")
    previousPageComponent: "qrc:/CommonComponents/Pages/AddressesIdentificationPage.qml"
    ready: testsRunnerId.testsResult
	nextPageComponent: "qrc:/CommonComponents/Pages/UutIdentificationPage.qml"
	processing: testsRunnerId.isRunning
	description: "This tests will check the communication with i2c devices.\nAll tests needs to pass to continue further."
	onCancel: preTestsController.abort()
	content: TestsRunner {
		id: testsRunnerId
		anchors.fill: parent
		testsController: preTestsController
		setChannelsMaskVisible: false
	}
}

