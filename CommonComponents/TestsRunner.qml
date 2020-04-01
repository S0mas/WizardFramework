import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.11

Item {
	property bool summaryButtonVisible: false
	property bool setChannelsMaskVisible: true
	property bool testsResult: false
	property bool isRunning: false
	property string runningTest: "None"
	property string summary: ""
	property var testsController

	ColumnLayout {
		anchors.fill: parent
		spacing: 5
		LogsArea {
			id: logsArea
			clipboardObject: clipboard
			Layout.fillHeight: true
			Layout.fillWidth: true
		}

		RowLayout {
			id: buttonsRowId
			Layout.minimumHeight: 25
			Layout.maximumHeight: 25
			Layout.fillWidth: true
			width: parent.width
			layoutDirection: Qt.RightToLeft
			spacing: 40
			Label {
				Layout.fillHeight: true
				text: "Already finished tests:"
				font.pointSize: 12
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}

			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 250
				text: "Error Handling Options"
				onClicked: errorHandlingSelectionDialogId.open()
			}

			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 250
				text: "Set channel mask"
				enabled: !isRunning
				onClicked: setChannelsMaskDialogId.open()
				visible: setChannelsMaskVisible
			}

			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 100
				Layout.maximumWidth: 100
				text: "Summary"
				onClicked: summaryDialogId.open()
				visible: summaryButtonVisible
			}

			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 100
				Layout.maximumWidth: 100
				text: "Abort"
				enabled: isRunning
				onClicked: testsController.abortTests()
			}

			MyButton {
				id: pauseButtonId
				Layout.fillHeight: true
				Layout.minimumWidth: 100
				Layout.maximumWidth: 100
				text: "Pause"
				enabled: isRunning && !(text == "wait")
				onClicked: {
					text == "Pause" ? testsController.pauseTests() : testsController.resumeTests()
					text = "wait"
				}
				Connections {
					target: testsController
					onPaused: pauseButtonId.text = "Resume"
					onResumed: pauseButtonId.text = "Pause"
				}
			}

			MyButton {
				Layout.fillHeight: true
				Layout.minimumWidth: 100
				Layout.maximumWidth: 100
				text: "Run tests"
				enabled: !isRunning
				onClicked: {
					isRunning = true
					summary = ""
					logsArea.clear()
					runnedTestsListId.model.clear()
					testsController.startTests()
				}
			}
		}
		ScrollView {
			Layout.minimumHeight: 100
			Layout.maximumHeight: 100
			Layout.fillWidth: true
			background: Rectangle {
				anchors.fill: parent
				border.color: "black"
			}
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
					color: result == 0 ? "green" : result == 1 ? "red" : "orange"
					font.pointSize: 12
				}
			}
		}
		Label {
			Layout.minimumHeight: 25
			Layout.maximumHeight: 25
			Layout.fillWidth: true
			text: "Currently running test: " + runningTest
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignLeft
			font.pointSize: 12
			color: "blue"
		}
	}

	Connections {
		target: testsController
		onOneRunned: runningTest = name
		onOneFinished: runnedTestsListId.model.append({name: name, result: result})
		onCompleted: {
			runningTest = "all tests finished"
			testsResult = result
			isRunning = false;
		}
		onAborted: {
			isRunning = false
			runningTest = "test run aborted"
		}
		onLogMsg: logsArea.addLog(msg)
		onOneSummary: summary += msg
	}

	ConfirmationDialog{
		id: summaryDialogId
		text : summary
		width : 700
		title : "Tests summary"
	}

	ChannelsSelectionDialog {
		id: setChannelsMaskDialogId
		onAccepted: testsController.setChannelsStateOption(channelsState)
		title: "Select the channels that will be tested"
		channelsNo: channelsCount
	}

	ConfirmationDialog {
		id: userConfirmationDialogId
		width : 700
		onRejected : dataPromise.set()
		onAccepted : dataPromise.set()
	}

	ConfirmationDialog {
		id: userConfirmationOrDeclineDialogId
		width : 700
		standardButtons : Dialog.Ok | Dialog.Cancel
		onRejected: dataPromise.set(false)
		onAccepted: dataPromise.set(true)
	}

	ChannelsSelectionDialog {
		id: channelsStateDialogId
		standardButtons : Dialog.Ok
		channelsNo: channelsCount
		defaultState : false
		onRejected : dataPromise.set(channelsStateDialogId.channelsState)
		onAccepted : dataPromise.set(channelsStateDialogId.channelsState)
	}

	Dialog {
		id: errorHandleSelectionDialogId
		x: Math.round((parent.width - width) / 2)
		y: Math.round((parent.height - height) / 2)
		modal: true
		focus: true
		title: "Error occured!"
		width : 700
		property alias text: textAreaId.text
		property var dataPromise
		Column {	
			width: parent.width
			ScrollView {
				width: parent.width
				TextArea {
					width: parent.width
					id: textAreaId
					readOnly: true
					font.pointSize: 10
				}
			}
			Row {
				width: parent.width
				spacing: 1
				Button {
					width: parent.width /3
					text: "Ignore"
					onClicked: {
						errorHandleSelectionDialogId.dataPromise.set(0)
						errorHandleSelectionDialogId.close();
					}
				}
				Button {
					width: parent.width /3
					text: "Skip test"
					onClicked: {
						errorHandleSelectionDialogId.dataPromise.set(1)
						errorHandleSelectionDialogId.close();
					}
				}
				Button {
					width: parent.width /3
					text: "Stop test run"
					onClicked: {
						errorHandleSelectionDialogId.dataPromise.set(2)
						errorHandleSelectionDialogId.close();
					}
				}
			}
		}
		onRejected: errorHandleSelectionDialogId.dataPromise.set(0)
	}

	ConfirmationDialog {
		id: errorHandlingSelectionDialogId
		width : 700
		height: 150
		property int automaticErrorHandlingSelection: 0
		property bool manualErrorHandling: true
		onAccepted: errorHandlingSelectionDialogId.close()
		Row {
			id: rowId
			width: parent.width
			RadioButton {
				checked: errorHandlingSelectionDialogId.manualErrorHandling
				text: "manual"
				onCheckedChanged: if(checked) errorHandlingSelectionDialogId.manualErrorHandling = true
	
			}
			RadioButton {
				text: "auto: ignore"
				onCheckedChanged: if(checked) { 
					errorHandlingSelectionDialogId.manualErrorHandling = false
					errorHandlingSelectionDialogId.automaticErrorHandlingSelection = 0
				}
			}
			RadioButton {
				text: "auto: skip test"
				onCheckedChanged: if(checked) { 
					errorHandlingSelectionDialogId.manualErrorHandling = false
					errorHandlingSelectionDialogId.automaticErrorHandlingSelection = 1
				}
			}
			RadioButton {
				text: "auto: stop test run"
				onCheckedChanged: if(checked) { 
					errorHandlingSelectionDialogId.manualErrorHandling = false
					errorHandlingSelectionDialogId.automaticErrorHandlingSelection = 2
				}
			}
		}
	}
	
	Connections {
		target:userActionRequestController
		onRequestConfirmationReceived: {
			userConfirmationDialogId.text = msg
			userConfirmationDialogId.dataPromise = barrierPromise
			userConfirmationDialogId.open()
		}
		onRequestAcceptOrDeclineReceived: {
			userConfirmationOrDeclineDialogId.text = msg
			userConfirmationOrDeclineDialogId.dataPromise = dataPromise
			userConfirmationOrDeclineDialogId.open()
		}
		onRequestChannelsSelectionReceived: {
			channelsStateDialogId.title = msg
			channelsStateDialogId.enabledChannels = channelsStatuses
			channelsStateDialogId.dataPromise = dataPromise
			channelsStateDialogId.open()
		}
		onRequestErrorHandlingActionTypeReceived: {
			if(errorHandlingSelectionDialogId.manualErrorHandling) {
				errorHandleSelectionDialogId.text = errorMsg
				errorHandleSelectionDialogId.dataPromise = dataPromise
				errorHandleSelectionDialogId.open()
			}
			else
				dataPromise.set(errorHandlingSelectionDialogId.automaticErrorHandlingSelection)
		}
	}
}	