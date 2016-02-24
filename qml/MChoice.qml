import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property string text:""
    property alias textColor: textInput.color
    property alias availableValues: selectvalues.model
    ComboBox {
        id: selectvalues
        anchors.fill: parent
        onCurrentTextChanged: root.text = selectvalues.currentText
    }
}

