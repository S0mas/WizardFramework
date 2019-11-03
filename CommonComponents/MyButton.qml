import QtQuick 2.4
import QtQuick.Controls 2.4

Button {
	id: button
	height: 45
	font.family: "Helvetica"
	font.pointSize: 12
	background: Rectangle {
		color: button.down ? "#d6d6d6" : "#aac3c3"
		border.color: "#26282a"
		border.width: 1
		radius: 4
	}
}
