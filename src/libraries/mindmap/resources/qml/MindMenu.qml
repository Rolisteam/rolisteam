import QtQuick
import QtQuick.Controls

Menu {
    id: menu
    dim: true
    modal: true
    property QtObject ctrl
    MenuItem {
        text: qsTr("Add Root")
        onTriggered: ctrl.addNode("")
    }
    MenuSeparator { }
    MenuItem {
        text: qsTr("Save")

        onTriggered: {
            if(ctrl.filename)
                ctrl.saveFile();
            else
                saveDialog.open()
        }
    }
    MenuItem {
        text: qsTr("Remove Selection")
        enabled: ctrl.hasSelection
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
