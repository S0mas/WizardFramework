import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

GroupBox {
    property alias textInput : textInputId
	label: Label {
		font.pointSize: 12
		text: title
	}
	RowLayout {
		anchors.horizontalCenter: parent.horizontalCenter
		TextInput {
			id: textInputId
			Layout.fillHeight: true
			Layout.minimumWidth: 300
			font.pointSize: 14
			verticalAlignment: TextInput.AlignVCenter
			horizontalAlignment: TextInput.AlignHCenter
		}
	}
}