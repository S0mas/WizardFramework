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
	onNext: dataInterface.setActiveUser(userNameComboboxId.combobox.text)


    LabeledComboBox {
		id: userNameComboboxId
        label.text: "User name"
        combobox.model: usersNamesModel
        combobox.textRole: "display"
        combobox.onCurrentIndexChanged: ready = combobox.currentIndex != -1
		combobox.width: 300
    }

	Component.onCompleted: userNameComboboxId.combobox.currentIndex = parseInt(appSettings.get("userId", -1));
	Component.onDestruction: appSettings.set("userId", userNameComboboxId.combobox.currentIndex)
}
