import QtQuick 2.4
import QtQuick.Controls 2.4

ScrollView {
	property alias delegate : listViewId.delegate
	property alias model : listViewId.model
	ScrollBar.vertical.policy: contentHeight > height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
	ScrollBar.horizontal.policy: contentWidth > width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

	ListView {
		id: listViewId
		width: parent.width
		height: parent.height
		spacing: 30
	}
}


