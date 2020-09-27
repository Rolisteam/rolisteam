import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Universal 2.12

Flickable {
    id: flick
    property QtObject ctrl

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
        modal: true
        ColumnLayout {
            anchors.fill: parent
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
                x: position.x
                y: position.y
                color: Universal.foreground
                width: widthLink
                height: heightLink
                start: position
                end:last
                startBox: startBoxRole
                endBox: endBoxRole
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
                text : label
                visible: node.visible
                selected: node.selected
                buttonColor: Universal.foreground
                onAddChild: ctrl.addBox(node.id)
                onOpenChanged: ctrl.nodeModel.openNode(node.id, open)
                onReparenting: ctrl.reparenting(node,id)
                onSelectStyle: {
                    stylePopup.parent = nodeItem
                    stylePopup.open()
                }
                onClicked: {
                    if(mouse.modifiers & Qt.ControlModifier) {
                        selected ? ctrl.selectionCtrl.removeFromSelection(node) : ctrl.selectionCtrl.addToSelection(node)
                    }
                    else if(!selected){
                        ctrl.selectionCtrl.clearSelection()
                        ctrl.selectionCtrl.addToSelection(node)
                    }
                }

            }
        }

    }
}
