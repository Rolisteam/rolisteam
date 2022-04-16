import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T
import Customization 1.0

T.Button {
    id: control
    property QtObject style: Theme.styleSheet("Palette")

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 6
    horizontalPadding: padding + 2
    spacing: 6

    icon.width: 24
    icon.height: 24
    icon.color: control.checked || control.highlighted ? control.style.brightText :
                control.flat && !control.down ? (control.visualFocus ? control.style.highlight : control.style.windowText) : control.style.buttonText

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: control.checked || control.highlighted ? control.style.brightText :
               control.flat && !control.down ? (control.visualFocus ? control.style.highlight : control.style.windowText) : control.style.buttonText
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        visible: !control.flat || control.down || control.checked || control.highlighted
        color: Color.blend(control.checked || control.highlighted ? control.style.dark : control.style.button,
                                                                    control.style.mid, control.down ? 0.5 : 0.0)
        border.color: control.style.highlight
        border.width: control.visualFocus ? 2 : 0
    }
}

