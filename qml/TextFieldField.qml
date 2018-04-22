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
    property alias placeholderText: textField.placeholderText
    property string tooltip: ""
    TextField {
        id: textField
        anchors.fill: parent
        selectByMouse: true
        ToolTip.text: root.tooltip
        ToolTip.visible: root.tooltip.length >0 && activeFocus
        readOnly: root.readOnly
    }
}

