import QtQuick 2.13
import QtQuick.Controls 2.12

Column {
    property alias label : labelId
    property alias combobox : comboboxId

    Label {
        id: labelId
    }

    ComboBox {
        id: comboboxId
    }
}
