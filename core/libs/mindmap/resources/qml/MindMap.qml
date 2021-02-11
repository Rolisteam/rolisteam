import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Customization 1.0

Flickable {
    id: flick
    property QtObject ctrl
    property QtObject styleSheet: Theme.styleSheet("mindmapinteral")

    contentHeight: mapmind.height
    contentWidth: mapmind.width
    contentX: 0
    contentY: 0
    interactive: true
    boundsBehavior: Flickable.StopAtBounds

    ScrollBar.vertical: ScrollBar { }
    ScrollBar.horizontal: ScrollBar { }

    Shortcut {
        sequence: StandardKey.Undo
        onActivated:  ctrl.undo();
    }
    Shortcut {
        sequence: StandardKey.Redo
        onActivated: ctrl.redo();
    }

    Popup {
        id: stylePopup
        height: flick.styleSheet.popupHeight
        y: flick.styleSheet.popupY
        modal: true
        ColumnLayout {
            anchors.fill: parent
                icon.source: flick.styleSheet.addIcon
                icon.name: flick.styleSheet.removeIconName
                icon.source: flick.styleSheet.removeIconSource
            GroupBox {
                id: styleTab
                title: qsTr("Node Styles")
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: flickable.contentWidth
                contentHeight: flickable.contentHeight
                ScrollView {
                    id: flickable
                    anchors.fill: parent
                    contentWidth: grid.implicitWidth+15
                    contentHeight: grid.implicitHeight
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                    clip: true
                    GridLayout {
                        id: grid
                        columns: 3

                        Repeater {
                            model: ctrl.styleModel
                            Button {
                                id: control
                                width: 80
                                height: 30
                                background: Rectangle {
                                    radius: 8
                                    border.width: 1
                                    border.color: "black"
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: control.pressed ? colorTwo : colorOne }//
                                        GradientStop { position: 1.0; color: control.pressed ? colorOne : colorTwo }//
                                    }

                                }
                                contentItem: Text {
                                    color: textColor
                                    text: qsTr("Text")
                                }
                                onClicked: {
                                    stylePopup.parent.object.styleIndex = index
                                    stylePopup.close()
                                }
                            }
                        }
                    }
                }
            }

        }

    }

    Pane {
        id: mapmind
        x: 0
        y: 0
        width: Math.max(2000, ctrl.contentRect.width)
        height: Math.max(1000, ctrl.contentRect.height)

        scale: root.viewScale
        transformOrigin: Item.Center
        MouseArea {
            anchors.fill:parent
            acceptedButtons:Qt.LeftButton
            onClicked: ctrl.selectionCtrl.clearSelection()
        }
        Repeater {
            anchors.fill: parent
            model: ctrl.linkModel
            delegate: Link {
                x: startPointRole.x
                y: startPointRole.y
                width: widthLink
                height: heightLink
                start: startPointRole
                end: endPointRole
                visible: link.visible
                text: label
            }
        }
        Repeater {
            anchors.fill: parent
            model: ctrl.nodeModel
            delegate: Node {
                id: nodeItem
                x: position.x
                y: position.y
                object: node
                nodeStyle: ctrl.getStyle(node.styleIndex)
                focus: true
                text : node.text
                source: node.imageUri
                visible: node.visible
                selected: node.selected
                onAddChild: ctrl.addBox(node.id)
                onOpenChanged: ctrl.nodeModel.openNode(node.id, open)
                onReparenting: ctrl.reparenting(node,id)
                onAddCharacter: {
                    ctrl.addCharacterBox(node.id, name, source, color)
                }

                onSelectStyle: {
                    stylePopup.parent = nodeItem
                    stylePopup.open()
                }
                onClicked: {
                    if(mouse.modifiers & Qt.ControlModifier) {
                        selected ? ctrl.selectionCtrl.removeFromSelection(node) : ctrl.selectionCtrl.addToSelection(node)
                    }
                    else if(!selected){
                        root.ctrl.selectionCtrl.clearSelection()
                        root.ctrl.selectionCtrl.addToSelection(node)
                    }
                }

            }
        }

    }
}
