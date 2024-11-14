import QtQuick
import QtQuick.Templates as T
import QtQuick.Controls
import QtQuick.Controls.impl
import Customization

T.SplitView {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitContentHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitContentWidth + leftPadding + rightPadding)

    handle: Rectangle {
        color: T.SplitHandle.pressed ? control.style.mid : (T.SplitHandle.hovered ? control.style.midlight : control.style.button)
        implicitHeight: control.orientation === Qt.Horizontal ? control.height : 6
        implicitWidth: control.orientation === Qt.Horizontal ? 6 : control.width
    }
}
