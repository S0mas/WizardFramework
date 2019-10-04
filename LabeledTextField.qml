import QtQuick 2.13
import QtQuick.Controls 2.12

Row {
    property alias label : labelId
    property alias textField : textFieldId
    Label {
        id: labelId
    }

    TextField {
        id: textFieldId
    }
}
