import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl

Button {
    id: root
    property alias source: img.source
    property string tooltip: root.action ? root.action.text : ""
    implicitHeight: 32
    implicitWidth: 32
    contentItem: Image {
        id: img
        source: root.action?.icon.source
        fillMode: Image.PreserveAspectFit
        sourceSize.height: 32
        sourceSize.width: 32
        opacity: root.enabled ? 1.0 : 0.2
    }
    hoverEnabled: true

    ToolTip.delay: 1000
    ToolTip.timeout: 5000
    ToolTip.visible: hovered
    ToolTip.text: root.tooltip
    display: AbstractButton.IconOnly

    background: Rectangle {
        border.color: root.style.highlight
        border.width: root.visualFocus ? 2 : 0
        color: Color.blend(root.checked || root.highlighted ? root.style.dark : root.style.button, root.style.mid, root.down ? 0.5 : 0.0)
        implicitHeight: 24
        implicitWidth: 80
        visible: !root.flat || root.down || root.checked || root.highlighted
    }
}
