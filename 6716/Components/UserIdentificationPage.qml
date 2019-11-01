import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: userIdentificationPageId
    title: qsTr("User Identification")
    nextPageComponent: "MdaTestConfirmationPage.qml"
    previousPageComponent: ""
    ready: dataObject.getActiveUser() !== -1
	description: "Please, choose user name or add new one."

    LabeledComboBox {
        label.text: "User name"
        combobox.model: usersNamesModel
        combobox.textRole: "display"
        combobox.currentIndex: dataObject.getActiveUser()
        combobox.onCurrentIndexChanged: dataObject.setActiveUser(combobox.currentIndex)
        combobox.onActivated: ready = dataObject.getActiveUser() !== -1
		combobox.width: 300
    }
}
