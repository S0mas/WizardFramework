import QtQuick 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Dialog {
	id: channelMaskDialogId
	x: Math.round((parent.width - width) / 2)
	y: Math.round((parent.height - height) / 2)
	modal: true
	focus: true
	title: "Select yhe channels that will be tested"
	standardButtons: Dialog.Ok | Dialog.Cancel
	property string channelMask : "1111111111111111"
	Column {
		GridLayout {
			columns: 4
			Repeater {
				model: 16
				CheckBox {
					Layout.row: index / 4
					Layout.column: index % 4
					Layout.fillWidth: true
					Layout.fillHeight: true
					text: "Channel " + (index + 1)
					checked: true
					onCheckedChanged: checked ? channelMaskDialogId.toggleChannel(15-index, 1) : channelMaskDialogId.toggleChannel(15-index, 0)
					Connections {
						target: selectChannelsButtonId
						onClicked: checked = true
					}
					Connections {
						target: deselectChannelsButtonId
						onClicked: checked = false
					}
				}
			}
		}
		Row {
			width: parent.width
			height: 30
			MyButton {
				id: selectChannelsButtonId
				width: parent.width/2
				height: parent.height
				text: "Select all"
			}
			MyButton {
				id: deselectChannelsButtonId
				width: parent.width/2
				height: parent.height
				text: "Deselect all"
			}
		}
	}
	closePolicy: Popup.CloseOnEscape
	function toggleChannel(index, chr) {
		channelMask = channelMask.substr(0, index) + chr + channelMask.substr(index+1);
		console.log(channelMask)
	}
}
