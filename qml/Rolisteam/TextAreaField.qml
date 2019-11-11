import QtQuick 2.10
import QtQuick.Controls 2.12
import Rolisteam 1.0

ScrollView {
    id: root
    property alias hAlign: text.horizontalAlignment
    property alias vAlign: text.verticalAlignment
    property bool clippedText: false
    property alias text: text.text
    property bool backgroundVisible: true
    property alias backgroundColor: rect.color
    property alias color: text.color
    property string tooltip: ""
    property alias readOnly: text.readOnly
    property alias wrapMode: text.wrapMode
    property alias textFormat: text.textFormat
    property TextArea textArea: text
    property Field field: null
    clip: true
    TextArea {
        id:text
        height: root.height
        width: root.width
        background: Rectangle {
            id: rect
        }

        selectByMouse: true
        ToolTip.text: root.tooltip
        ToolTip.visible: root.tooltip.length >0 && hovered
        textFormat: TextEdit.RichText
        wrapMode: TextEdit.Wrap
        font: root.font
        onEditingFinished: field.value = text.text
    }
}
