import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls.Universal 2.12
import RMindMap 1.0
import Customization 1.0

Item {
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

    Universal.theme: root.darkMode ? Universal.Dark: Universal.Light

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
        anchors.rightMargin: 14
        anchors.topMargin: 14
        IconButton {//undo
            //source: _engineCtrl.undoIcon
            enabled: ctrl.canUndo
            onClicked: ctrl.undo()
        }
        IconButton {//redo
            //source: _engineCtrl.redoIcon
            enabled: ctrl.canRedo
            onClicked: ctrl.redo()
        }
        IconButton {
            //source: _engineCtrl.listIcon
            onClicked: drawer.open()
        }
    }

    Drawer {
        id: drawer
        edge: Qt.RightEdge
        width: root.styleSheet.drawerPropertion * root.width
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
                    model: root.ctrl.styleModel
                    currentIndex: 0
                    onCurrentIndexChanged: ctrl.defaultStyleIndex = currentIndex

                   contentItem: Rectangle {
                        radius: root.styleSheet.styleButtonRadius
                        width: root.styleSheet.styleButtonWidth
                        height: root.styleSheet.styleButtonHeight
                        border.width: root.styleSheet.borderSize
                        border.color: root.styleSheet.borderColor
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: ctrl.getStyle(combo.currentIndex).colorOne }
                            GradientStop { position: 1.0; color: ctrl.getStyle(combo.currentIndex).colorTwo }
                        }
                        Text {
                            anchors.centerIn: parent
                            color: root.ctrl.getStyle(combo.currentIndex).textColor
                            text: qsTr("Text")
                        }
                    }


                    delegate: ItemDelegate {
                        width: combo.width
                        height: root.styleSheet.delegateHeight

                        Rectangle {
                            radius: root.styleSheet.styleButtonRadius
                            width: root.styleSheet.styleButtonWidth
                            height: root.styleSheet.styleButtonHeight
                            anchors.centerIn: parent
                            border.width: root.styleSheet.borderSize
                            border.color: root.styleSheet.borderColor
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
