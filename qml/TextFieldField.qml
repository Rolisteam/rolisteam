import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 1.3

Rectangle {
    id:root
    property alias text : textField.text
    property alias textColor: textField.textColor
    property alias availableValues: selectvalues.model
    property alias hAlign: textField.horizontalAlignment
    property alias vAlign: textField.verticalAlignment
    property bool clippedText: false
    TextField {
        id: textField
        anchors.fill: parent
        selectByMouse: true
    }
}

