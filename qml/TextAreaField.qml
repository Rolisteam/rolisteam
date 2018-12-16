import QtQuick 2.10
import QtQuick.Controls 2.3

Rectangle {
    id:root
    property alias text : textArea.text
    property alias textColor: textArea.color
    property alias font: textArea.font
    property alias wrapMode : textArea.wrapMode
    property alias hAlign: textArea.horizontalAlignment
    property alias vAlign: textArea.verticalAlignment
    property bool clippedText: false
    property bool readOnly: false
    property bool backgroundVisible: true
    property alias background: textArea.background
    property string tooltip: ""
    property alias placeholderText: textArea.placeholderText
    TextArea {
        id: textArea
        anchors.fill: parent
        selectByMouse: true
        readOnly: root.readOnly
        ToolTip.text: root.tooltip
        ToolTip.visible: root.tooltip.length >0 && hovered
        textFormat: TextEdit.RichText
    }
}
