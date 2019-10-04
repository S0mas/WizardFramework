import QtQuick 2.13
import QtQuick.Controls 2.12

Row {
    property alias label : labelId
    property alias textField : textFieldId
    signal save(string str)
    signal refresh()
    Label {
        id: labelId
    }

    TextField {
        id: textFieldId
        enabled: checkboxId.checked
    }

    CheckBox {
        id: checkboxId
        text: "Enable update procedure"
        onCheckedChanged: refresh()
    }

    RoundButton {
        id: buttonId
        text: "Update"
        enabled: checkboxId.checked
        onClicked: save(textFieldId.text)

    }
    Component.onCompleted: refresh()
}
