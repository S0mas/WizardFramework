import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12

Button {
	id: buttonId
    style: ButtonStyle {
        background: Rectangle {
            border.width: mouseAreaId.containsMouse ? 2 : 1
            border.color: control.enabled ? palette.mid : palette.midlight
            radius: 5
            gradient: Gradient {
                GradientStop { position: 0.0; color: control.enabled ? (mouseAreaId.containsPress ? palette.alternateBase : (mouseAreaId.containsMouse ? palette.midlight : palette.midlight)) : paletteDisabled.button }
                GradientStop { position: 0.5; color: control.enabled ? (mouseAreaId.containsPress ? palette.alternateBase : (mouseAreaId.containsMouse ? palette.button : palette.button)) :   paletteDisabled.button }
                GradientStop { position: 1.0; color: control.enabled ? (mouseAreaId.containsPress ? palette.alternateBase : (mouseAreaId.containsMouse ? palette.button : palette.midlight)) : paletteDisabled.button }
            }
        }
		label: Text {
			renderType: Text.NativeRendering
			verticalAlignment: Text.AlignVCenter
			horizontalAlignment: Text.AlignHCenter
			font.pointSize: 12
			text: control.text
			color: control.enabled ? palette.text : paletteDisabled.text
		}
    }
	MouseArea {
		id: mouseAreaId
		anchors.fill: parent
		onClicked: buttonId.clicked()
		hoverEnabled: true
	}
	SystemPalette {
		id: palette
		colorGroup: SystemPalette.Active
	}
		SystemPalette {
		id: paletteDisabled
		colorGroup: SystemPalette.Disabled
	}
}


