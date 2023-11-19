import QtQuick 2.0
import QtQuick.Controls 2.12

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
}
