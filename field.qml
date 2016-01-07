import QtQuick 2.0
import QtQuick 2.5

Item {
    id:root
    property Alias text:textInput.text
    TextInput {
        id: textInput
        anchors.fill: parent
        selectByMouse: true
    }

}

