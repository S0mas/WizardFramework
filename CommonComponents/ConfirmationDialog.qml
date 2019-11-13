import QtQuick 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Dialog {
	id: userConfirmationDialogId
	property alias text : dialogTextId.text
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
}
