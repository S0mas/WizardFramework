import QtQuick 2.4
import QtQuick.Controls 2.4

ScrollView {
	property alias delegate : listViewId.delegate
	property alias model : listViewId.model
	ScrollBar.vertical.policy: ScrollBar.AsNeeded
	ScrollBar.horizontal.policy: ScrollBar.AsNeeded
	GridView {
		id: runnedTestsListId
		cellHeight: 25
		model: ListModel{}
		delegate: Label {
			height: runnedTestsListId.cellHeight
			width: runnedTestsListId.cellWidth
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			text: {
				runnedTestsListId.cellWidth = runnedTestsListId.cellWidth > contentWidth ? runnedTestsListId.cellWidth : contentWidth
				return name
			}
			color: result ? "green" : "red"
			font.family: "Helvetica"
			font.pointSize: 12
		}
	}
}