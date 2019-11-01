import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: testCompletitionPageId
    title: qsTr("Test Completition")
    nextPageComponent: ""
	previousPageComponent: "TestValidationPage.qml"
    ready: true
	description: "List of available tests and their result, green - passed, red - failed, black - not runned."
	ScrollList {
		model: testsModel
		delegate: Row {
			height: 25
			width: parent.width
			Label {
				height: parent.height
				text: testsModel.getTestName(index)
				color: getColor(index)
				font.family: "Helvetica"
				font.pointSize: 12
			}
			function getColor(index) {
				if(testsModel.getTestResult(index))
					return "green"
				else if(testsModel.getTestRunned(index))
					return "red"
				return "black"
			}
		}
	}
}
