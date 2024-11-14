import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Controls.impl
import QtQuick.Templates as T
import Customization

T.Drawer {
    id: control

    property QtObject style: Theme.styleSheet("Palette")

    bottomPadding: control.edge === Qt.TopEdge
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, contentHeight + topPadding + bottomPadding)
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, contentWidth + leftPadding + rightPadding)
    leftPadding: control.edge === Qt.RightEdge
    parent: T.Overlay.overlay
    rightPadding: control.edge === Qt.LeftEdge
    topPadding: control.edge === Qt.BottomEdge

    T.Overlay.modal: Rectangle {
        color: Color.transparent(control.style.shadow, 0.5)
    }
    T.Overlay.modeless: Rectangle {
        color: Color.transparent(control.style.shadow, 0.12)
    }
    background: Rectangle {
        color: control.style.base

        Rectangle {
            readonly property bool horizontal: control.edge === Qt.LeftEdge || control.edge === Qt.RightEdge

            color: control.style.dark
            height: horizontal ? parent.height : 1
            width: horizontal ? 1 : parent.width
            x: control.edge === Qt.LeftEdge ? parent.width - 1 : 0
            y: control.edge === Qt.TopEdge ? parent.height - 1 : 0
        }
    }
    enter: Transition {
        SmoothedAnimation {
            velocity: 5
        }
    }
    exit: Transition {
        SmoothedAnimation {
            velocity: 5
        }
    }
}
