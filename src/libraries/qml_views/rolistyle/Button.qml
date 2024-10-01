import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl
import QtQuick.Controls.Fusion
import QtQuick.Controls.Fusion.impl
import Customization

T.Button {
    id: control
    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 4
    spacing: 6

    icon.width: 16
    icon.height: 16

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.style.buttonText
    }

    background: ButtonPanel {
        implicitWidth: 80
        implicitHeight: 24

        control: control
        visible: !control.flat || control.down || control.checked || control.highlighted || control.visualFocus
            || (enabled && control.hovered)
    }
}



