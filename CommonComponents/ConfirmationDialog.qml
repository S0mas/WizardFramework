import QtQuick 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Dialog {
	id: userConfirmationDialogId
	property alias text : textAreaId.text
	property var dataPromise
	x: Math.round((parent.width - width) / 2)
	y: Math.round((parent.height - height) / 2)
	modal: true
	focus: true
	title: "Action required!"
	standardButtons: Dialog.Ok
	ScrollView {
		anchors.fill: parent
		TextArea {
			id: textAreaId
			anchors.fill: parent
			readOnly: true
			font.pointSize: 10
		}
	}
}
