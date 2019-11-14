import QtQuick 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Dialog {
	id: userConfirmationDialogId
	property alias text : textAreaId.text
	x: Math.round((parent.width - width) / 2)
	y: Math.round((parent.height - height) / 2)
	title: "Action required!"
	standardButtons: Dialog.Ok
	contentItem: Rectangle {
		ScrollView {
			id: scrollViewId
			anchors.fill: parent
			TextArea {
				id: textAreaId
				anchors.fill: parent
				readOnly: true
				font.family: "Helvetica"
				font.pointSize: 10
			}
		}
	}
}
