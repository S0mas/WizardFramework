import QtQuick 2.13
import QtQuick.Controls 2.12

MyPage {
    id: uutIdentificationPageId
    title: qsTr("UUT Identification")
    nextPageComponent: "MdaTestConfirmationPage.qml"
    previousPageComponent: "UserIdentificationPage.qml"
    ready: true
    Column {
        DataBaseField {
            label.text: "Subtype:"
            onSave: dataObject.setSubtype(textEdit.text)
            onRefresh: textField.text = dataObject.getSubtype()
        }

        DataBaseField {
            label.text: "Serial number"
            onSave: dataObject.setSerialNumber(textEdit.text)
            onRefresh: textField.text = dataObject.getSerialNumber()
        }

        DataBaseField {
            label.text: "Product revision"
            onSave: dataObject.setProductRevision(textEdit.text)
            onRefresh: textField.text = dataObject.getProductRevision()
        }
    }
}
