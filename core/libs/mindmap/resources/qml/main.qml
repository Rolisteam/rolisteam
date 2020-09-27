import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls.Universal 2.12
import RMindMap 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 400
    property real viewScale: 1
    property int idx: 0
    property bool darkMode: false

    onDarkModeChanged: _engineCtrl.nightMode = root.darkMode

    Universal.theme: root.darkMode ? Universal.Dark: Universal.Light

    MindMapController {
        id: ctrl
    }

    FileDialog {
        id: importDialog
        title: qsTr("Please, select a file to import")
        folder: shortcuts.home
        selectMultiple: false
        nameFilters: ["Text file (*.txt)"]
        onAccepted: {
            ctrl.importFile(importDialog.fileUrl)
            close()
        }
        onRejected: close()
    }

    FileDialog {
        id: openDialog
        title: qsTr("Load Mindmap from File")
        folder: shortcuts.home
        selectMultiple: false
        nameFilters: ["Rolisteam MindMap (*.rmap)"]
        onAccepted: {
            ctrl.setFilename(openDialog.fileUrl)
            ctrl.loadFile();
            close()
        }
        onRejected: close()
    }

    FileDialog {
        id: saveDialog
        title: qsTr("Save Mindmap into File")
        folder: shortcuts.home
        selectExisting: false
        selectMultiple: false
        defaultSuffix: "rmap"
        nameFilters: ["Rolisteam MindMap (*.rmap)"]
        onAccepted: {
            ctrl.setFilename(saveDialog.fileUrl)
            ctrl.saveFile();
            close()
        }
        onRejected: close()
    }



    MindMap {
        anchors.fill: parent
        ctrl: ctrl
    }
    MindMenu {
        id: menu
        ctrl: ctrl
    }


    RowLayout{
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.topMargin: 14
        IconButton {//undo
            source: _engineCtrl.undoIcon
            enabled: ctrl.canUndo
            onClicked: ctrl.undo()
        }
        IconButton {//redo
            source: _engineCtrl.redoIcon
            enabled: ctrl.canRedo
            onClicked: ctrl.redo()
        }
        IconButton {
            source: _engineCtrl.listIcon
            onClicked: drawer.open()
        }
    }

    Drawer {
        id: drawer
        edge: Qt.RightEdge
        width: 0.33 * root.width
        height: root.height
        ColumnLayout {
            anchors.fill: parent

            Switch {
                text: qsTr("Night Mode")
                checked: false
                onCheckedChanged: root.darkMode = checked
            }
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: qsTr("Default Style")
                }
                ComboBox {
                    id: combo
                    model: ctrl.styleModel
                    currentIndex: 0
                    onCurrentIndexChanged: ctrl.defaultStyleIndex = currentIndex

                   contentItem: Rectangle {
                        radius: 8
                        width: 80
                        height: 15
                        border.width: 1
                        border.color: "black"
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: ctrl.getStyle(combo.currentIndex).colorOne }
                            GradientStop { position: 1.0; color: ctrl.getStyle(combo.currentIndex).colorTwo }
                        }
                        Text {
                            anchors.centerIn: parent
                            color: ctrl.getStyle(combo.currentIndex).textColor
                            text: qsTr("Text")
                        }
                    }


                    delegate: ItemDelegate {
                        width: combo.width
                        height: 20

                        Rectangle {
                            radius: 8
                            width: 80
                            height: 15
                            anchors.centerIn: parent
                            border.width: 1
                            border.color: "black"
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: colorOne }
                                GradientStop { position: 1.0; color: colorTwo }
                            }
                            Label {
                                anchors.centerIn: parent
                                color: model.textColor
                                text: qsTr("Text")
                            }
                        }
                    }
                }
            }

            Label {
               property string logs
               text: qsTr("Errors:\n%1").arg(logs)
               Layout.fillHeight: true
               Layout.fillWidth: true
            }
        }
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
