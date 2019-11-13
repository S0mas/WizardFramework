import QtQuick 2.4
import QtQuick.Controls 2.4

Row {
    property alias label : labelId
    property alias textInput : textInputId
	property int holderWidth : 200
	property int holderWidth2 : 150
	width: parent.width
	height: 45

	PlaceHolder {
		height: parent.height
		width: holderWidth2
	}

    Label {
		id: labelId
		height: parent.height
		font.family: "Helvetica"
		font.pointSize: 12
		verticalAlignment: Text.AlignVCenter
    }

	PlaceHolder {
		height: parent.height
		width: labelId.width > holderWidth ? 0 : holderWidth - labelId.width
	}

    TextInput {
		id: textInputId
		height: parent.height
		width: 300
		font.family: "Helvetica"
		font.pointSize: 12
		verticalAlignment: TextInput.AlignVCenter
    }
}
