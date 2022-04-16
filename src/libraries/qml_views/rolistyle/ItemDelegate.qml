import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import Customization 1.0

T.ItemDelegate {
    id: control
    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    padding: 12
    spacing: 8

    icon.width: 24
    icon.height: 24
    icon.color: control.style.text

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display
        alignment: control.display === IconLabel.IconOnly || control.display === IconLabel.TextUnderIcon ? Qt.AlignCenter : Qt.AlignLeft

        icon: control.icon
        text: control.text
        font: control.font
        color: control.highlighted ? control.style.highlightedText : control.style.text
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 30
        visible: control.down || control.highlighted || control.visualFocus
        color: control.highlighted ? control.style.highlight : Color.blend(control.down ? control.style.midlight : control.style.light,
                           control.style.highlight, control.visualFocus ? 0.75 : 0.15)
    }
}
