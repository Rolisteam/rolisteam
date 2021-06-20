import QtQuick 2.0
import QtQuick.Controls 2.12

Button {
    id: root
    property alias source: img.source
    implicitHeight: 32
    implicitWidth: 32
    contentItem: Image {
        id: img
        source: root.source
        fillMode: Image.PreserveAspectFit
        sourceSize.height: 32
        sourceSize.width: 32
        opacity: root.enabled ? 1.0 : 0.2
    }
    display: AbstractButton.IconOnly
}
