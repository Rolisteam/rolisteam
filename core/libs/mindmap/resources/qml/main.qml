import QtQuick 2.15
import QtQuick.Window 2.2
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import RMindMap 1.0
import Customization 1.0

Pane {
    id: root
    visible: true
    width: 800
    height: 400
    property QtObject styleSheet: Theme.styleSheet("mindmap")
    property real viewScale: 1
    property int idx: 0
    property bool darkMode: false
    property QtObject ctrl: _ctrl

    onDarkModeChanged: Theme.nightMode = root.darkMode

    MindMap {
        anchors.fill: parent
        ctrl: root.ctrl
    }
    MindMenu {
        id: menu
        ctrl: root.ctrl
    }


    RowLayout{
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: root.styleSheet.margin
        anchors.topMargin: root.styleSheet.margin

        IconButton {//undo
            source: root.styleSheet.undoIcon
            enabled: ctrl.canUndo
            onClicked: ctrl.undo()
        }
        IconButton {//redo
            source: root.styleSheet.redoIcon
            enabled: ctrl.canRedo
            onClicked: ctrl.redo()
        }
        IconButton {
            source: root.styleSheet.listIcon
            onClicked: drawer.open()
        }
    }

    DrawerMenu {
        id: drawer
        width: root.styleSheet.drawerPropertion * root.width
        height: root.height
        ctrl: root.ctrl
        onDarkModeChanged: root.darkMode = darkMode
    }

    MouseArea {
        anchors.fill:parent
        acceptedButtons:Qt.MiddleButton | Qt.RightButton
        propagateComposedEvents: true
        onClicked:{
            menu.x = mouse.x
            menu.y = mouse.y
            menu.open()
        }
        onWheel: {
            var step = (wheel.modifiers & Qt.ControlModifier) ? 0.1 : 0.01
            if(wheel.angleDelta.y>0)
            {
                root.viewScale = Math.min(root.viewScale+step,2.0)
            }
            else
                root.viewScale = Math.max(root.viewScale-step,0.2)
        }
    }
}
