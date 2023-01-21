import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import mindmap
import Customization

GridLayout{
    id: _root
    columns: 4
    property QtObject styleSheet: Theme.styleSheet("mindmap")
    property alias addPackage: _pack.checked
    property alias addArrow: addArrow.checked
    property alias automaticSpacing: spacing.checked
    property bool showLittleMap: true
    required property QtObject ctrl
    property var actions:[]


    signal openDrawer()
    signal exportScene()

    IconButton {//undo
        source: _root.styleSheet.undoIcon
        enabled: _root.ctrl.canUndo
        onClicked: _root.ctrl.undo()
        tooltip: qsTr("Undo")
    }
    IconButton {//redo
        source: _root.styleSheet.redoIcon
        enabled: _root.ctrl.canRedo
        onClicked: _root.ctrl.redo()
        tooltip: qsTr("Redo")
    }
    IconButton {
        source: _root.styleSheet.saveIcon
        onClicked: _root.ctrl.modified = true
        tooltip: qsTr("Save Map")
    }

    IconButton {
        tooltip: qsTr("Open Drawer")
        source: _root.styleSheet.listIcon
        onClicked: _root.openDrawer()
    }
    IconButton {//add node
        id: _addRoot
        source: _root.styleSheet.addIcon
        tooltip: qsTr("Add Root")
        onClicked: _root.ctrl.addNode("")
    }
    IconButton {//add package
        id: _pack
        source: _root.styleSheet.editIcon
        checkable: true
        tooltip: qsTr("Add Package")
    }

    IconButton {//Add gray arrow
        id: addArrow
        tooltip: qsTr("Add Arrow")
        source: _root.styleSheet.addGrayArrow
        checkable: true
    }
    IconButton {//remove selection
        tooltip: qsTr("Remove Selection")
        source: _root.styleSheet.trashIcon
        enabled: _root.ctrl.hasSelection
        onClicked: _root.ctrl.removeSelection()
    }

    IconButton {
        tooltip: qsTr("Refresh")
        source: _root.styleSheet.refreshIcon
        onClicked: _root.ctrl.refresh()
    }
    IconButton {//
        tooltip: qsTr("Export As PNG")
        source: _root.styleSheet.exportIcon
        onClicked: _root.exportScene()
    }
    IconButton {//
        tooltip: _root.showLittleMap ? qsTr("Hide small map") : qsTr("Show small map")
        source: _root.styleSheet.litteMapIcon
        onClicked: _root.showLittleMap = !_root.showLittleMap
    }
    IconButton {//
        id: spacing
        tooltip: qsTr("Automatic Spacing")
        checkable: true
        source: _root.styleSheet.spacingIcon
    }

    Repeater {
        model: _root.actions
        IconButton {
            action: modelData
        }
    }

}
