import QtQuick 2.4
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11

Dialog {
	id: channelsStateDialogId
	x: Math.round((parent.width - width) / 2)
	y: Math.round((parent.height - height) / 2)
	modal: true
	focus: true
	standardButtons: Dialog.Ok | Dialog.Cancel
	property bool defaultState: true
	property int channelsNo : 1
	property var channelsState : new Array(channelsNo).fill(defaultState)
	property var enabledChannels : new Array(channelsNo).fill(true)
	property var dataPromise
	Column {
		GridLayout {
			columns: Math.ceil(Math.sqrt(channelsNo))
			Repeater {
				model: channelsNo
				CheckBox {
					Layout.row: index / Math.ceil(Math.sqrt(channelsNo))
					Layout.column: index % Math.ceil(Math.sqrt(channelsNo))
					Layout.fillWidth: true
					Layout.fillHeight: true
					text: "Channel " + (index + 1)
					checked: defaultState
					enabled: enabledChannels[index]
					onCheckedChanged: channelsState[index] = checked
					Connections {
						target: selectChannelsButtonId
						onClicked: if(enabled) checked = true
					}
					Connections {
						target: deselectChannelsButtonId
						onClicked: if(enabled) checked = false
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
}
