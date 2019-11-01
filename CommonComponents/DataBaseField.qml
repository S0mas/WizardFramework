import QtQuick 2.4
import QtQuick.Controls 2.4

Row {
    property alias label : labelId
    property alias textField : textFieldId
	property int holderWidth : 200
	property int holderWidth2 : 150
    signal save(string str)
    signal refresh()
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
		anchors.verticalCenter: parent.verticalCenter
		verticalAlignment: Text.AlignVCenter
    }

	PlaceHolder {
		height: parent.height
		width: labelId.width > holderWidth ? 0 : holderWidth - labelId.width
	}

    TextField {
        id: textFieldId
		height: parent.height
        enabled: checkboxId.checked
		font.family: "Helvetica"
		font.pointSize: 12
		anchors.verticalCenter: parent.verticalCenter
    }

    CheckBox {
        id: checkboxId
		height: parent.height
        text: "Enable update procedure"
        onCheckedChanged: refresh()
		font.family: "Helvetica"
		font.pointSize: 12
		anchors.verticalCenter: parent.verticalCenter
    }

    MyButton {
        id: buttonId
        text: "Update"
        enabled: checkboxId.checked
        onClicked: save(textFieldId.text)
		anchors.verticalCenter: parent.verticalCenter
    }
    Component.onCompleted: refresh()
}
