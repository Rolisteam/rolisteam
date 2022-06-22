import QtQuick 2.10
import QtQuick.Controls 2.4

Label {
    id:root
    property alias backgroundColor: rect.color
    property alias hAlign: root.horizontalAlignment
    property alias vAlign: root.verticalAlignment
    property bool clippedText: false
    property bool readOnly: false
    property string tooltip: ""
    ToolTip.text: root.tooltip
    ToolTip.visible: root.tooltip.length >0 && activeFocus
    background: Rectangle {
        id:rect
    }
    wrapMode: Text.Wrap
}

