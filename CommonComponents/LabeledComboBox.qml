import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

GroupBox {
    property alias model : comboboxId.model
	property alias text : comboboxId.currentText
	property alias selection : comboboxId.currentIndex
	signal selectedItemChanged()
	label: Label {
		font.pointSize: 12
		text: title
	}
	RowLayout {
		anchors.horizontalCenter: parent.horizontalCenter
		ComboBox {
			id: comboboxId
			Layout.fillHeight: true
			Layout.minimumWidth: 300
			textRole: "display"
			onCurrentIndexChanged: selectedItemChanged()
			contentItem: Label {
				font.pointSize: 14
				horizontalAlignment: TextInput.AlignHCenter
				verticalAlignment: TextInput.AlignVCenter
				text: parent.currentText
			}
		}
	}
}

