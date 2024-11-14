import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.Popup {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    padding: 12

    T.Overlay.modal: Rectangle {
        color: Color.transparent(control.style.shadow, 0.5)
    }
    T.Overlay.modeless: Rectangle {
        color: Color.transparent(control.style.shadow, 0.12)
    }
    background: Rectangle {
        border.color: control.style.dark
        color: control.style.window
    }
}
