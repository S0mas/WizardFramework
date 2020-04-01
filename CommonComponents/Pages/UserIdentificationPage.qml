import QtQuick 2.12
import QtQuick.Controls 2.12
import "../../CommonComponents"

MyPage {
    id: userIdentificationPageId
    title: qsTr("User Identification")
    nextPageComponent: "qrc:/CommonComponents/Pages/AddressesIdentificationPage.qml"
    previousPageComponent: ""
    ready: userNameComboboxId.selection != -1
	description: "Please, choose user name or add new one."
    content: LabeledComboBox {
		id: userNameComboboxId
		anchors.centerIn: parent
        title: "User name"
        model: usersNamesModel
        onSelectedItemChanged: ready = selection != -1
    }

	Component.onCompleted: userNameComboboxId.selection = parseInt(appSettings.get("userId", -1));
	Component.onDestruction: appSettings.set("userId", userNameComboboxId.selection)
}
