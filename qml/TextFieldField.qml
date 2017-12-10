import QtQuick 2.0
import QtQuick 2.4
import QtQuick.Controls 2.0

Rectangle {
    id:root
    property alias text : textField.text
    property alias textColor: textField.color
    property alias hAlign: textField.horizontalAlignment
    property alias font : textField.font
    property alias vAlign: textField.verticalAlignment
    property bool clippedText: false
    property bool readOnly: false
    TextField {
        id: textField
        anchors.fill: parent
        selectByMouse: true
        readOnly: root.readOnly
    }
}

