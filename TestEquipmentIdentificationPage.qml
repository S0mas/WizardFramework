import QtQuick 2.13
import QtQuick.Controls 2.12

MyPage {
    id: testEquipmentIdentificationPageId
    title: qsTr("Test Equipment Identification")
    nextPageComponent: "TestSelectionPage.qml"
    previousPageComponent: "MdaTestConfirmationPage.qml"
    ready: true
    Column {
        LabeledTextField {
            label.text: "RaspberryPi id:"
        }

        LabeledTextField {
            label.text: "Testing software version"
        }

        LabeledTextField {
            label.text: "Testerboard serial number:"
        }

        LabeledTextField {
            label.text: "Termination board serial number:"
        }

        LabeledTextField {
            label.text: "Breakoutboard serial number:"
        }

        LabeledTextField {
            label.text: "Power supply serial number:"
        }
    }
}
