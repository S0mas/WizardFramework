import QtQuick 2.4
import QtQuick.Controls 2.4

ScrollView {
	property alias delegate : listViewId.delegate
	property alias model : listViewId.model
	width: parent.width
	height: parent.height
	ListView {
		id: listViewId
		width: parent.width
		height: parent.height
	}
}


