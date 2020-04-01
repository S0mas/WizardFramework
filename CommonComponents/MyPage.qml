import QtQuick 2.10
import QtQuick.Controls 2.10

Page {
    property string nextPageComponent: ""
    property string previousPageComponent: ""
    property bool ready: true
	property bool processing: false
	property string description: "MY PAGE DEFAULT DESCRIPTION"
	default property alias content: placeholder.children
	signal cancel
	signal back
	signal next

    header: Column {
		spacing: 10
		padding: 5
		Text {
			text: title
			font.pointSize: 20
		}
		Text {
			text: description
			font.pointSize: 12
		}
	}
	Item {
        id: placeholder
		anchors.fill: parent
    }
}
