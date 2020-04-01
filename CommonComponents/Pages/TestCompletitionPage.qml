import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testCompletitionPageId
    title: qsTr("Test Completition")
    nextPageComponent: ""
	previousPageComponent: "qrc:/CommonComponents/Pages/TestValidationPage.qml"
    ready: true
	description: "List of available tests and their result, green - passed, red - failed, black - not runned."
}
