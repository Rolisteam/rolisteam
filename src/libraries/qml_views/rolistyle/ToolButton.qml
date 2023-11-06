import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates  as T
import Customization

T.ToolButton {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    spacing: 6

    icon.width: 16
    icon.height: 16
    icon.color: control.style.buttonText

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.style.buttonText
    }


    background: Rectangle {
        id: rectId
        implicitWidth: 20
        implicitHeight: 20

        color: control.enabled && (control.highlighted || control.checked) ? control.style.button : "transparent"

        opacity: control.down || control.hovered ? 1.0 : 0.4
    }
}
