import QtQuick
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.ItemDelegate {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    icon.color: control.style.text
    icon.height: 24
    icon.width: 24
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    padding: 12
    spacing: 8

    background: Rectangle {
        color: control.highlighted ? control.style.highlight : Color.blend(control.down ? control.style.midlight : control.style.light, control.style.highlight, control.visualFocus ? 0.75 : 0.15)
        implicitHeight: 30
        implicitWidth: 100
        visible: control.down || control.highlighted || control.visualFocus
    }
    contentItem: IconLabel {
        alignment: control.display === IconLabel.IconOnly || control.display === IconLabel.TextUnderIcon ? Qt.AlignCenter : Qt.AlignLeft
        color: control.highlighted ? control.style.highlightedText : control.style.text
        display: control.display
        font: control.font
        icon: control.icon
        mirrored: control.mirrored
        spacing: control.spacing
        text: control.text
    }
}
