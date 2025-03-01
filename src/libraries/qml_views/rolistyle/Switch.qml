import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls
import QtQuick.Controls.impl
import Customization

T.Switch {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding, implicitIndicatorHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)
    padding: 6
    spacing: 6

    opacity: control.enabled ? 1.0 : 0.5

    contentItem: CheckLabel {
        color: control.style.text
        font: control.font
        leftPadding: control.indicator && !control.mirrored ? control.indicator.width + control.spacing : 0
        rightPadding: control.indicator && control.mirrored ? control.indicator.width + control.spacing : 0
        text: control.text
    }
    indicator: PaddedRectangle {
        color: control.checked ? control.style.checked : control.style.axis
        implicitHeight: 28
        implicitWidth: 56
        leftPadding: 0
        padding: (height - 16) / 2
        radius: 8
        rightPadding: 0
        x: text ? (control.mirrored ? control.width - width - control.rightPadding : control.leftPadding) : control.leftPadding + (control.availableWidth - width) / 2
        y: control.topPadding + (control.availableHeight - height) / 2

        Rectangle {
            //border.color: control.visualFocus ? control.palette.highlight : control.enabled ? control.palette.mid : control.palette.midlight
            border.color: control.visualFocus ? control.style.highlight : control.enabled ? control.style.mid : control.style.midlight
            border.width: control.visualFocus ? 2 : 1
            //color: control.down ? control.palette.light : control.palette.window
            color: control.down ? control.style.light : control.style.window
            height: 28
            radius: 16
            width: 28
            x: Math.max(0, Math.min(parent.width - width, control.visualPosition * parent.width - (width / 2)))
            y: (parent.height - height) / 2

            Behavior on x {
                enabled: !control.down

                SmoothedAnimation {
                    velocity: 200
                }
            }
        }
    }
}
