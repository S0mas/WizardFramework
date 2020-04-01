import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

GroupBox {
    property alias textInput : textInputId
    signal save(string str)
    signal refresh()
	label: Label {
		font.pointSize: 12
		text: title
	}
	RowLayout {
		anchors.horizontalCenter: parent.horizontalCenter
		spacing: 10
		TextInput {
			id: textInputId
			Layout.fillHeight: true
			Layout.minimumWidth: 300
			enabled: checkboxId.checked
			font.pointSize: 14
			verticalAlignment: TextInput.AlignVCenter
			horizontalAlignment: TextInput.AlignHCenter
		}

		CheckBox {
			id: checkboxId
			Layout.fillHeight: true
			Layout.minimumWidth: 30
			Layout.maximumWidth: 30
			text: "Enable update procedure"
			onCheckedChanged: refresh()
			font.pointSize: 14
		}

		MyButton {
			id: updateButtonId
			text: "Update"
			Layout.fillHeight: true
			Layout.minimumWidth: implicitWidth
			Layout.maximumWidth: implicitWidth
			enabled: checkboxId.checked
			onClicked: save(textInputId.text)
		}
		Component.onCompleted: refresh()
	}
}