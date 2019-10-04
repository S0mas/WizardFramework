import QtQuick 2.13
import QtQuick.Controls 2.12

MyPage {
    id: userIdentificationPageId
    title: qsTr("User Identification")
    nextPageComponent: "UutIdentificationPage.qml"
    previousPageComponent: ""
    ready: dataObject.getActiveUser() !== -1

    LabeledComboBox {
        label.text: "Choose user name or add new one"
        combobox.model: usersNamesModel
        combobox.textRole: "display"
        combobox.currentIndex: dataObject.getActiveUser()
        combobox.onCurrentIndexChanged: dataObject.setActiveUser(combobox.currentIndex)
        combobox.onActivated: ready = dataObject.getActiveUser() !== -1
    }
}
