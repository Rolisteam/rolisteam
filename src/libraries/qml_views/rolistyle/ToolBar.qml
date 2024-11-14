import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.ToolBar {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    background: Rectangle {
        implicitHeight: 40
        color: control.style.button
    }
}

