import QtQuick
import QtQuick.Controls
import Rolisteam

ScrollView {
    id: root
    property alias hAlign: textarea.horizontalAlignment
    property alias vAlign: textarea.verticalAlignment
    property bool clippedText: false
    property bool backgroundVisible: true
    property alias backgroundColor: rect.color
    property alias color: textarea.color
    property string tooltip: ""
    property alias readOnly: textarea.readOnly
    property alias wrapMode: textarea.wrapMode
    property alias textFormat: textarea.textFormat
    property TextArea textArea: textarea
    property alias contentText: textarea.text

    signal editingFinished(string text)

    clip: true
    TextArea {
        id: textarea
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
        onEditingFinished: root.editingFinished(textarea.text)
    }
}
