import QtQuick 2.4
import QtQuick.Controls 2.4
import "../../CommonComponents"

MyPage {
    id: userIdentificationPageId
    title: qsTr("User Identification")
    nextPageComponent: "MdaTestConfirmationPage.qml"
    previousPageComponent: ""
    ready: userNameComboboxId.combobox.currentIndex != -1
	description: "Please, choose user name or add new one."

    LabeledComboBox {
		id: userNameComboboxId
        label.text: "User name"
        combobox.model: usersNamesModel
        combobox.textRole: "display"
        combobox.onCurrentIndexChanged: dataInterface.setActiveUser(combobox.currentIndex)
        combobox.onActivated: ready = combobox.currentIndex != -1
		combobox.width: 300
		Connections {
			target: dataInterface
			onActiveUser: userNameComboboxId.combobox.currentIndex = value
		}
    }
}
