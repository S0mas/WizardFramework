import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.12

ApplicationWindow {
    id:top
    visible: true
	visibility: "Maximized"
	minimumWidth: 1000
	minimumHeight: 980
	header: Image {
		width: 300
		height: 120
		source: "qrc:///images/bustec.jpg"
		anchors.horizontalCenter: parent.horizontalCenter
	}
	GroupBox {
		anchors.fill: parent
		padding: 5
		Loader {
			id: pageLoader
			anchors.fill: parent
			source: "qrc:/CommonComponents/Pages/UserIdentificationPage.qml"
			function nextPage() {
				source = item.nextPageComponent
			}

			function prevPage() {
				source = item.previousPageComponent
			}

			function hasNextPage() {
				return item.nextPageComponent != ""
			}

			function hasPrevPage() {
				return item.previousPageComponent != ""
			}

			function nextPageEnabled() {
				return item.ready && !item.processing && hasNextPage()
			}

			function prevPageEnabled() {
				return !item.processing && hasNextPage()
			}

			function cancel() {
				item.cancel()
				Qt.callLater(Qt.quit)
			}
		}
	}
	footer: GroupBox {
		padding: 10
		background: Rectangle {}
			RowLayout {
			anchors.fill: parent
			Layout.fillWidth: true
			Layout.minimumHeight: 25
			spacing: 40
			layoutDirection: Qt.RightToLeft
			MyButton {
				Layout.fillHeight: true
				Layout.maximumWidth: 100
				Layout.minimumWidth: 100
				text: "Next"
				onClicked: pageLoader.nextPage()
				enabled: pageLoader.nextPageEnabled()
			}
			MyButton {
				Layout.fillHeight: true
				Layout.maximumWidth: 100
				Layout.minimumWidth: 100
				text: "Back"
				visible: pageLoader.hasPrevPage()
				onClicked: pageLoader.prevPage()
				enabled: pageLoader.prevPageEnabled()
			}
			MyButton {
				Layout.fillHeight: true
				Layout.maximumWidth: 100
				Layout.minimumWidth: 100
				text: pageLoader.hasNextPage() ? "Cancel" : "Finish"
				onClicked: pageLoader.cancel()
			}
			TextField {
				Layout.fillWidth: true
				Layout.fillHeight: true
				background: Rectangle {
					anchors.fill: parent
					color: devicesController.connectionStatus ? "lightgreen" : "lightgrey"
					radius: 16
				}
				text: devicesController.connectionStatus ? "Connected" : "Disconnected / Connection Problems"
				readOnly: true
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
		}
	}
}
