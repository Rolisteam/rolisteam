import QtQuick
import QtQuick.Controls

Menu {
    id: menu
    dim: true
    modal: true
    property QtObject ctrl
    signal saveMap()
    MenuItem {
        text: qsTr("Add Root")
        enabled: menu.ctrl.readWrite
        onTriggered: ctrl.addNode("")
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Save")
        enabled: menu.ctrl.readWrite
        onTriggered: {
            console.log("Item save map")
            menu.saveMap()
        }
    }
    MenuItem {
        text: qsTr("Remove Selection")
        enabled: ctrl.hasSelection &&menu.ctrl.readWrite
        onTriggered: {
            ctrl.removeSelection();
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Undo")
        enabled: ctrl.canUndo
        onTriggered: {
            ctrl.undo()
        }
    }
    MenuItem {
        text: qsTr("Redo")
        enabled: ctrl.canRedo
        onTriggered: {
            ctrl.redo()
        }
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Center")
        enabled: false//TODO Fix it
        onTriggered: {
            ctrl.centerItems(menu.mindMapWidth, menu.mindMapHeight)
        }
    }
    MenuSeparator { }
   // MenuSeparator { }
    MenuItem {
        text: qsTr("Automatic Spacing")
        checkable: true
        checked: ctrl.spacing
        onTriggered: ctrl.spacing = checked
    }
}
