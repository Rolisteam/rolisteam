import QtQuick 2.0
import QtQuick 2.4

Rectangle {
    id:root
    property alias text:textInput.text
    TextInput {
        id: textInput
        anchors.fill: parent
        selectByMouse: true
    }

}

