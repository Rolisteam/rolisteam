import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls.impl
import Customization

T.Button {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    icon.height: 24
    icon.width: 24
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    padding: 4
    spacing: 6

    background: ButtonPanel {
        control: control
        implicitHeight: 24
        implicitWidth: 80
        visible: !control.flat || control.down || control.checked || control.highlighted || control.visualFocus || (enabled && control.hovered)
    }
    contentItem: IconLabel {
        color: control.style.buttonText
        display: control.display
        font: control.font
        icon: control.icon
        mirrored: control.mirrored
        spacing: control.spacing
        text: control.text
    }
}
